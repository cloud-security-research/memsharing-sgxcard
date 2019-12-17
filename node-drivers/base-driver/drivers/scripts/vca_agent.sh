#!/bin/bash
#
# Intel VCA Software Stack (VCASS)
#
# Copyright(c) 2016-2017 Intel Corporation.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2, as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# The full GNU General Public License is included in this distribution in
# the file called "COPYING".
#
# Intel VCA Scripts.
#

export PATH=$PATH:/sbin
export PATH=$PATH:/bin

FINISH=false

cleanup()
{
	FINISH=true
}

trap cleanup SIGHUP

set_have_ip()
{
	echo "net_device_ready" > /sys/class/vca/vca/state
}

set_have_no_ip()
{
	echo "net_device_no_ip" > /sys/class/vca/vca/state
}

set_have_no_device()
{
	echo "net_device_down" > /sys/class/vca/vca/state
}

set_have_device()
{
	echo "net_device_up" > /sys/class/vca/vca/state
}

find_default_interface()
{
	ETH=eth0
	if [ -h /sys/class/net/$ETH/brport/bridge ]; then
		export INTERFACE=`readlink /sys/class/net/$ETH/brport/bridge | rev | cut -d '/' -f 1 |rev`
	else
		export INTERFACE=$ETH
	fi
}

start_dhclient()
{
	find_default_interface
	dhclient $@ -R subnet-mask,broadcast-address,time-offset,root-path,routers,domain-name-servers,nis-domain,nis-domain,ntp-servers $INTERFACE
	return $?
}

check_dhclient()
{
	if [ ! -e "/sys/class/vca/vca/net_config" ]; then return; fi
	NEED_DHCLIENT=`cat /sys/class/vca/vca/net_config | grep dhclient`
	if [ -z "$NEED_DHCLIENT" ]; then return; fi
	pidof dhclient > /dev/null
	#exit if dhclient already works
	if [ $? -eq 0 ]; then return; fi
	start_dhclient -nw
}

state_manager()
{
	IFACE=$1
	if [ -z "$IFACE" ] || [ ! -e "/sys/class/vca/vca/state" ]; then
		return
	fi
	STATE=$( cat /sys/class/vca/vca/state)
	if [ -z "$STATE" ]; then
		return
	fi

	HAVE_IFACE=`ip link show | grep $IFACE`
	IP=$( ip addr show dev $IFACE |grep "inet " )
	case $STATE in
		net_device_down)
			if [ "$HAVE_IFACE" != "" ]; then
				set_have_device
			fi
			;;
		net_device_ready)
			if [ -z "$IP" ]; then
				set_have_no_ip
			fi
			;;
		net_device_no_ip | dhcp_error)
			if [ -n "$IP" ]; then
				set_have_ip
			fi
			;;
		*)
			if [ "$HAVE_IFACE" == "" ]; then
				set_have_no_device
				break
			fi

			if [ -z "$IP" ]; then
				set_have_no_ip
			else
				set_have_ip
			fi
			;;
	esac
}

run_sys_config()
{
	while [ ! -e "/sys/class/vca/vca/sys_config" ]; do
		sleep 1
	done

	bash "/sys/class/vca/vca/sys_config"
}


listener()
{
	INPUT=''
	while ! $FINISH; do
		INPUT=$( cat /sys/class/vca/vca/csa_mem )
		find_default_interface
		case $INPUT in
			sensors)
				sensors coretemp-isa-0000 > /sys/class/vca/vca/csa_mem
				;;
			ip)
				ip addr show dev $INTERFACE > /tmp/csa_mem_dump
				cat /tmp/csa_mem_dump > /sys/class/vca/vca/csa_mem
				rm -f /tmp/csa_mem_dump
				;;
			ip_stats)
				ip -s -s link ls $INTERFACE > /sys/class/vca/vca/csa_mem
				;;
			dhcp_renew)
				if [[ $(mount | grep " / type nfs") ]]; then
					echo "Cannot renew DHCP for systems with rootfs mounted via nfs (i.e. persistent OS), as this may lead to OS crash" > /sys/class/vca/vca/csa_mem
					continue
				fi
				echo "dhcp_in_progress" > /sys/class/vca/vca/state
				dhclient -r $INTERFACE && start_dhclient -timeout 15 -1
				if [ $? -ne 0 ]; then
					echo "dhcp_error" > /sys/class/vca/vca/state
					echo "Couldn't renew DHCP lease" > /sys/class/vca/vca/csa_mem
				else
					echo "dhcp_done" > /sys/class/vca/vca/state
					echo "DHCP lease renewed" > /sys/class/vca/vca/csa_mem
				fi
				continue
				;;
			vm_mac)
				OUTPUT="VM MAC address not available. VM not running."
				if [ -f /etc/vca_vm_mac ]; then
					if [[ $(ifconfig | egrep 'xenif0|vnet0') ]]; then
						OUTPUT_TMP=$(cat /etc/vca_vm_mac)
							if [ "$OUTPUT_TMP" ]; then
								OUTPUT=$OUTPUT_TMP
							fi
					fi
				fi
				echo $OUTPUT > /sys/class/vca/vca/csa_mem
				;;
			os_shutdown)
				echo "OS Shutdown requested by user" > /sys/class/vca/vca/csa_mem
				halt -p
				;;
			os_reboot)
				echo "OS Reboot requested by user" > /sys/class/vca/vca/csa_mem
				reboot
				;;
		esac
		state_manager $INTERFACE
		check_dhclient
	done
}

start()
{
	echo "os_ready" > /sys/class/vca/vca/state
	yes "" | sensors-detect > /dev/null
	listener &
	run_sys_config &
}

stop()
{
	pkill -SIGHUP vca_agent
}

case $1 in
	start)
		start
		;;
	stop)
		stop
		;;
	listener)
		listener
		;;
esac
