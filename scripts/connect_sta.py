#!/usr/bin/env python3
__author__ = 'Pedro Heleno Isolani'
__copyright__ = 'Copyright 2020, SDN WiFi STA Agent'
__version__ = '1.0'
__maintainer__ = 'Pedro Heleno Isolani'
__email__ = 'pedro.isolani@uantwerpen.be'

import subprocess

from optparse import OptionParser

'''
    STA connect script
'''
parser = OptionParser()
parser.add_option('-i', '--ip', type='string', dest='ip',
                  help='IP address [default: %default]', default='192.168.2.20')
parser.add_option('-I', '--interface', type='string', dest='interface',
                  help='WiFi interface [default: %default]', default='wls33')
parser.add_option('-n', '--netmask', type='string', dest='netmask',
                  help='Netmask [default: %default]', default='255.255.255.0')
parser.add_option('-c', '--config', type='string', dest='config',
                  help='Config [default: %default]', default='wpa_supplicant/wpa_supplicant.conf')
parser.add_option('-s', '--sta', type='string', dest='sta',
                  help='STA ID [default: %default]', default='1')

(options, args) = parser.parse_args()


def run_cmd(cmd):
    print('running', cmd)
    subprocess.call(cmd)
    print('done!')


# Setting the wireless interface up
# sudo ifconfig wls33 192.168.2.20 netmask 255.255.255.0 up
cmd = ['ifconfig', options.interface, options.ip, 'netmask', options.netmask, 'up']
run_cmd(cmd)

# sudo wpa_supplicant -B -c /etc/wpa_supplicant/wpa_supplicant.conf -i wls33
cmd = ['wpa_supplicant', '-B', '-c', options.config, '-i', options.interface]
run_cmd(cmd)

# starting click
cmd = ['screen', '-d', '-m', '-S', 'click', './userlevel/click', 'conf/sta' + options.sta +'.click']
subprocess.call(cmd)
run_cmd(cmd)


