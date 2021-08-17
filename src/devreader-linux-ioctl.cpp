/*
 * nload
 * real time monitor for network traffic
 * Copyright (C) 2001 - 2012 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "devreader-linux-ioctl.h"
#include "stringutils.h"
#include "settingstore.h"
#include "setting.h"

#include <fstream>
#include <list>
#include <string>
#include <stdexcept>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <alloca.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>

using namespace std;

DevReaderLinuxIoctl::DevReaderLinuxIoctl(const string& deviceName)
    : DevReader(deviceName)
    , m_fd(-1)
    , m_n_stats(0)
    , m_datain_id(-1), m_dataout_id(-1)
    , m_packetin_id(-1), m_packetout_id(-1)
    , m_errorsin_id(-1), m_errorsout_id(-1)
    , m_dropsin_id(-1), m_dropsout_id(-1)
{
    if (!init()) {
        close(m_fd);
        m_fd = -1;
    }
}

bool DevReaderLinuxIoctl::init()
{
    // get all strings of ETH_SS_STATS, find rx_byte / tx_byte
    this->m_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (this->m_fd < 0)
        //throw std::runtime_error("create socket failed");
        return false;

    struct ifreq ifr;

    { // get count of ETH_SS_STATS
        struct {
            struct ethtool_sset_info hdr;
            unsigned int buf[1];
        } sset_info;
        
        sset_info.hdr.cmd = ETHTOOL_GSSET_INFO;
        sset_info.hdr.reserved = 0;
        sset_info.hdr.sset_mask = 1ULL << ETH_SS_STATS;

        memset(&ifr, 0, sizeof(ifr));
        strcpy(ifr.ifr_name, this->m_deviceName.c_str());
        ifr.ifr_data = (void*)&sset_info;
        int ret = ioctl(this->m_fd, SIOCETHTOOL, &ifr);
        if (0 != ret) {
            //throw std::runtime_error("ETHTOOL_GSSET_INFO(ETH_SS_STATS) failed");
            return false;
        }
        this->m_n_stats = sset_info.hdr.sset_mask ? sset_info.hdr.data[0] : 0;
    }

    { // get names of stats
        struct ethtool_gstrings *strings;
        strings = (ethtool_gstrings*)calloc(1, sizeof(*strings) + this->m_n_stats * ETH_GSTRING_LEN);
        if (!strings)
            //throw std::bad_alloc();
            return false;

        strings->cmd = ETHTOOL_GSTRINGS;
        strings->string_set = ETH_SS_STATS;
        strings->len = m_n_stats;

        memset(&ifr, 0, sizeof(ifr));
        strcpy(ifr.ifr_name, this->m_deviceName.c_str());
        ifr.ifr_data = (void*)strings;
        int ret = ioctl(this->m_fd, SIOCETHTOOL, &ifr);
        if (0 != ret) {
            //throw std::runtime_error("ETHTOOL_GSTRINGS(ETH_SS_STATS) failed");
            return false;
        }
        bool statofphy = SettingStore::get("StatisticsOfPhyBytes");
        bool statofrdma = SettingStore::get("StatisticsOfRDMABytes");
        std::string nameofrxprop = SettingStore::get("NameOfRxProp");
        std::string nameoftxprop = SettingStore::get("NameOfTxProp");
        //fprintf(stderr, "%s statofrdma=%d\n", this->m_deviceName.c_str(), statofrdma);
        for (int i = 0; i < (int)strings->len; ++i) {
            char* p = i*ETH_GSTRING_LEN + (char*)&strings->data[0];
            if (SettingStore::get("ListStatProps"))
                printf("%s[%d] = %s\n", this->m_deviceName.c_str(), i, p);
            if (statofphy) {
                if (strcmp(p, "rx_bytes_phy") == 0) {
                    this->m_datain_id = i;
                } else if (strcmp(p, "tx_bytes_phy") == 0) {
                    this->m_dataout_id = i;
                } else if (strcmp(p, "rx_bytes_nic") == 0) {
                    this->m_datain_id = i;
                } else if (strcmp(p, "tx_bytes_nic") == 0) {
                    this->m_dataout_id = i;
                } else if (this->m_datain_id == -1 && strcmp(p, "rx_bytes") == 0) {
                    this->m_datain_id = i;
                } else if (this->m_dataout_id == -1 && strcmp(p, "tx_bytes") == 0) {
                    this->m_dataout_id = i;
                }
            } if (statofrdma) {
                if (strcmp(p, "rx_vport_rdma_unicast_bytes") == 0) {
                    this->m_datain_id = i;
                } else if (strcmp(p, "tx_vport_rdma_unicast_bytes") == 0) {
                    this->m_dataout_id = i;
                }
            } else {
                if (!nameofrxprop.empty()) {
                    if (strcmp(p, nameofrxprop.c_str()) == 0) {
                        this->m_datain_id = i;
                    }
                } else if (strcmp(p, "rx_bytes") == 0) {
                    this->m_datain_id = i;
                }
                if (!nameoftxprop.empty()) {
                    if (strcmp(p, nameoftxprop.c_str()) == 0) {
                        this->m_dataout_id = i;
                    }
                } else if (strcmp(p, "tx_bytes") == 0) {
                    this->m_dataout_id = i;
                }
            }
            if (strcmp(p, "rx_packets") == 0) {
                this->m_packetin_id = i;
            } else if (strcmp(p, "tx_packets") == 0) {
                this->m_packetout_id = i;
            } else if (strcmp(p, "rx_total_frames") == 0) {
                this->m_packetin_id = i;
            } else if (strcmp(p, "tx_total_frames") == 0) {
                this->m_packetout_id = i;
            }
        }
        free(strings);
    }
    return true;
}

DevReaderLinuxIoctl::~DevReaderLinuxIoctl()
{
    close(this->m_fd);
}

bool DevReaderLinuxIoctl::isAvailable()
{
    // test ETHTOOL_GSSET_INFO on interface lo
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
        return false;
    struct ifreq ifr;
    struct {
        struct ethtool_sset_info hdr;
        unsigned int buf[1];
    } sset_info;
    
    sset_info.hdr.cmd = ETHTOOL_GSSET_INFO;
    sset_info.hdr.reserved = 0;
    sset_info.hdr.sset_mask = 1ULL << ETH_SS_STATS;

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, "lo");
    ifr.ifr_data = (void*)&sset_info;
    int ret = ioctl(fd, SIOCETHTOOL, &ifr);
    if (0 != ret) {
        close(fd);
        return false;
    }
    close(fd);
    return true;
}

void DevReaderLinuxIoctl::readFromDevice(DataFrame& dataFrame)
{
    if(m_deviceName.empty() || m_fd == -1)
        return;

    // read device data
    struct ifreq ifr;
    struct ethtool_stats *stats;

    size_t sz_stats = sizeof(struct ethtool_stats) + m_n_stats * sizeof(unsigned long long);
    stats = (ethtool_stats*)alloca(sz_stats);
    memset(stats, 0, sz_stats); 
    stats->cmd = ETHTOOL_GSTATS;
    stats->n_stats = m_n_stats;

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, m_deviceName.c_str());
    ifr.ifr_data = (void*)stats;
    int ret = ioctl(m_fd, SIOCETHTOOL, &ifr);
    if (0 != ret) {
        return;
    }

    // assign data
    unsigned long long bytesIn = m_datain_id >= 0 ? stats->data[m_datain_id] : 0;
    unsigned long long packetsIn = m_packetin_id >= 0 ? stats->data[m_packetin_id] : 0;
    unsigned long long errorsIn = m_errorsin_id >= 0 ? stats->data[m_errorsin_id] : 0;
    unsigned long long dropsIn = m_dropsin_id >= 0 ? stats->data[m_dropsin_id] : 0;
    unsigned long long bytesOut = m_dataout_id >= 0 ? stats->data[m_dataout_id] : 0;
    unsigned long long packetsOut = m_packetout_id >= 0 ? stats->data[m_packetout_id] : 0;
    unsigned long long errorsOut = m_errorsout_id >= 0 ? stats->data[m_errorsout_id] : 0;
    unsigned long long dropsOut = m_dropsout_id >= 0 ? stats->data[m_dropsout_id] : 0;

        
    dataFrame.setTotalDataIn(bytesIn);
    dataFrame.setTotalDataOut(bytesOut);

    dataFrame.setTotalPacketsIn(packetsIn);
    dataFrame.setTotalPacketsOut(packetsOut);

    dataFrame.setTotalErrorsIn(errorsIn);
    dataFrame.setTotalErrorsOut(errorsOut);

    dataFrame.setTotalDropsIn(dropsIn);
    dataFrame.setTotalDropsOut(dropsOut);

    dataFrame.setValid(true);
}

