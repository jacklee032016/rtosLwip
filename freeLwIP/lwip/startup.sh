# /bin/bash


# Run me using "source setup-tapif" to get exported PRECONFIGURED_TAPIF variable
# Alternatively, add "export PRECONFIGURED_TAPIF=tap0" to ~/.bashrc

# http://backreference.org/2010/03/26/tuntap-interface-tutorial/

# After executing this script, start unixsim/simhost.
# Enter 192.168.0.2 or "http://simhost.local/" (Zeroconf)
# in your webbrowser to see simhost webpage.

# add both interface for TX and RX tests


sudo ip tuntap add dev tap0 mode tap user `whoami`
sudo ip link set tap0 up
sudo ip addr add 192.168.166.1/24 dev tap0

export PRECONFIGURED_TAPIF_TX=tap0

echo $PRECONFIGURED_TAPIF_TX


sudo ip tuntap add dev tap1 mode tap user `whoami`
sudo ip link set tap1 up
sudo ip addr add 192.168.167.1/24 dev tap1

export PRECONFIGURED_TAPIF_RX=tap1

echo $PRECONFIGURED_TAPIF_RX


