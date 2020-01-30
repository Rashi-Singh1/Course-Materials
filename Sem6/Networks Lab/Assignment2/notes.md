## Pointers for Lab:

# Question 1 : 

	a)	To specify no. of echo requests - 
			ping -c count www.iitg.ernet.in

	b)	To set time interval(in sec) btw 2 successive pings - 
			ping -i interval(in sec) www.iitg.ernet.in
			*	min value of $interval : 0.2 sec for non-super-users
			*	default value for all users - 1 sec

	c)	To send packets without waiting for reply -
			ping -l count www.iitg.ernet.in
			*	this preloads $count no of packets
			*	max value of count : 3 for non-super-users
			##	mention about flood (with 0 interval for superuser)

	d)	To specify packet size - 
			ping -s packetSize(in bytes) www.iitg.ernet.in
			*	Adds 8 bytes of ICMP header data
			*	For packetSize = 32, total size = 40 bytes


# Question 2 : 

	#DONE

# Question 3 : 

	a)	Packet Loss Rate : 
			Command 1 : 0%
			Command 2 : 0%

	b)	Min Latency :
			Command 1 : 0.814 ms
			Command 2 : 1.72 ms
		Max Latency : 
			Command 1 : 46.6 ms
			Command 2 : 113 ms
		Mean Latency :
			Command 1 : 5.59603 ms
			Command 2 : 3.19398 ms
		Median Latency :
			Command 1 : 4.85 ms
			Command 2 : 2.01 ms


# Question 4 : 
	
	a)	Notes on ifconfig :

			stands for I nter F ace CONFIG uration 
			IFCONFIG - 
				shows the list of network interfaces that are "UP" (enabled)
				Normal invocation - ifconfig interface [address [parameters]]

				enp4s0f1:        				flags=4099			<UP,BROADCAST,MULTICAST>  	mtu 1500
				(interface name)													  		  	(maximum number of octets the interface is able to handle in one transaction)
				| | |		        			ether                  a8:1e:84:55:ab:57      	txqueuelen 1000  (Ethernet)
				v | |		        			(ethernet interface)   (H/w addr, MAC addr)	  	(limits the number of packets in the transmission queue in the interface's device driver)
				en| |   --> ethernet  					   	  		   (Addr assigned to the LAN card)
				  v |							
				  p4|   --> bus number (4)		RX packets 0  bytes 0 (0.0 B)
				    v  							RX errors 0  dropped 0  overruns 0  frame 0
				    s0f1 --> slot number (0f1) 	TX packets 0  bytes 0 (0.0 B)			    
								      			TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
												(RX : Received, TX : Transmitted) 

				wlp3s0:							flags=4163	<UP,								BROADCAST,				RUNNING,MULTICAST>  mtu 1500
				(wl : wireless)				  				(the network interface is enabled) 	(supports Broadcast)
				(rest is same as above)						*   UP makes an interface accessible to the IP layer.
															*	RUNNING means the network driver has been loaded and has initialized the interface. UP means the network interface is activated i.e. it has an address and routing table assigned.
															* 	Multicast addresses are like Ethernet broadcast addresses, except that instead of automatically including everybody, the only people who receive packets sent to a multicast address are those programmed to listen to it

										        inet 10.42.0.161  			netmask 255.255.255.0                        broadcast 10.42.0.255
										        (IPv4 addr)       			(Network Mask for the associated IPv4 addr)  (Broadcast Addr)
										        (32 bit numeric IP addr)
										        (These addr were set when the machine booted, can also be set using ifconfig)

										        inet6 fe80::ed82:63a2:ba94:42a3  prefixlen 64  	scopeid 0x20<link>
										        (IPv6 addr)									   	(The scope of the IPv6 addr is not global, it is the link local address)
										        (128 bit alphanumeric IP addr) 				 	(Prefix length specifies the number of bits in the IP address that are to be used as the subnet mask)
										        ether 3c:a0:67:6c:14:2d  txqueuelen 1000  (Ethernet)
										        RX packets 12519  bytes 15344697 (15.3 MB)
										        RX errors 0  dropped 0  overruns 0  frame 0
										        TX packets 5747  bytes 1354683 (1.3 MB)
										        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

		        lo: 							flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
				(computer's ref to itself)		inet 127.0.0.1  netmask 255.0.0.0
				(virtual network interface)     inet6 ::1  		prefixlen 128  scopeid 0x10<host>
				(to comm with itself)			(replace the 
				(used mainly for diagnostics 	remaining bits with 0)
				and troubleshooting, and to 
				connect to servers running 
				on the local machine)
												
										        loop  txqueuelen 1000  (Local Loopback)
										        RX packets 402  bytes 34644 (34.6 KB)
										        RX errors 0  dropped 0  overruns 0  frame 0
										        TX packets 402  bytes 34644 (34.6 KB)
										        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
    b)	Instructions using ifconfig :

		*	ifconfig -a
			-	shows all network interfaces including the DOWN ones

		*	ifconfig <interface-name> UP
			ifconfig <interface-name> DOWN
			-	enables/disables the mentioned interface respectively

		*	ifconfig <interface-name> mtu count

		*	ifconfig <interface-name> netmask addr
			-	assigns given netmask addr to the given interface

		*	ifconfig <interface-name> broadcast addr
			-	assigns given broadcast addr to the given interface

		*	ifconfig <interface-name> addr
			-	assigns given addr as IP to the given interface

		*	ifconfig <interface-name> promisc
		*	ifconfig <interface-name> -promisc
			-	enables/disables promiscuous mode respectively
			-	in this mode, the driver does not check whether the packet is meant/not meant for itself and simply accepts all packets

	c)	Route :

			To show and manipulate kernel's IP routing table
			## 	mention about - setting up static routes to specific hosts/networks.

			Normal invocation - route [-f] [-p] [command [destination] [mask subnetmask] [gateway] [metric costmetric]]

			Result of {route} to show data in human friendly form : 
				Kernel IP routing table
				Destination     						Gateway         			Genmask         				Flags Metric Ref    		Use 							Iface
				(network dest of the route)				(default ga..)				(Netmask for dest)											(count of lookups for a route)	(Interface to which packets for this route will be sent)
														(0.0.0.0 if not set)		(255.255.255.255 for host,
              																		 0.0.0.0 for default route

				*	Gateways regulate traffic between two dissimilar networks, while routers regulate traffic between similar networks.
				*	The metric option assigns an integer cost metric (distance to target in hops)(that ranges from 1 to 9999) which you can use to calculate the fastest, most reliable, and least expensive routes.
				*	Possible flags include
						U (route is up)
						H (target is a host)
						G (use gateway)
						R (reinstate route for dynamic routing)
						D (dynamically installed by daemon or redirect)
						M (modified from routing daemon or redirect)
						A (installed by addrconf)
						C (cache entry)
						!  (reject route)
				default			         				strix-15        			0.0.0.0         				UG    20600  0        		0 								wlp3s0
				10.42.0.0			       				0.0.0.0         			255.255.255.0   				U     600    0        		0 								wlp3s0
	
			Result of {route -n} to show data in raw form :  
				Kernel IP routing table
				Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
				0.0.0.0         10.42.0.1       0.0.0.0         UG    20600  0        0 wlp3s0
				10.42.0.0       0.0.0.0         255.255.255.0   U     600    0        0 wlp3s0

	d)	Instructions using Route : 

		route -n
		*	show  numerical  addresses instead of trying to determine symbolic host names.

		route -e
		*	use  netstat(8)-format  for displaying the routing table.

		route add -host 172.16.112.3 reject
		*	Rejecting routing to a particular host

		route add -net 127.0.0.0 netmask 255.0.0.0 metric 1024 dev lo
		*	adds the normal loopback entry, using  netmask  255.0.0.0  and associated with the "lo" device

# Question 5 : 

	#PAUL

# Question 6 : 


x - represents  : zero window
162.125.19.131	bolt.v.dropbox.com  ****
104.16.99.29	cfl.dropboxstatic.com.cdn.cloudflare.net  ****
162.125.248.1	dropbox.com
162.125.82.5	edge-block-previews-video-live.dropbox-dns.com  **
104.16.100.29	cfl.dropboxstatic.com.cdn.cloudflare.net
162.125.82.1	www.dropbox-dns.com  *****
162.125.82.6	edge-block-www.dropbox-dns.com ****

ip.addr==162.125.19.131	
ip.addr==104.16.99.29	
ip.addr==162.125.248.1	
ip.addr==162.125.82.5	
ip.addr==104.16.100.29	
ip.addr==162.125.82.1	
ip.addr==162.125.82.6	

143.204.229.15	assets.dropbox.com
162.125.19.131	bolt.v.dropbox.com
104.16.99.29	cfl.dropboxstatic.com.cdn.cloudflare.net
143.204.229.110	assets.dropbox.com
143.204.229.19	assets.dropbox.com
104.16.100.29	cfl.dropboxstatic.com.cdn.cloudflare.net
162.125.80.6	edge-block-www.dropbox-dns.com
143.204.229.69	assets.dropbox.com
162.125.80.5	edge-block-previews-video-live.dropbox-dns.com
162.125.82.1	www.dropbox-dns.com
162.125.82.6	edge-block-www.dropbox-dns.com


HOST IP addresses
ip.addr==143.204.229.15	||ip.addr==162.125.19.131	||ip.addr==104.16.99.29	||ip.addr==143.204.229.110	||ip.addr==143.204.229.19	||ip.addr==104.16.100.29	||ip.addr==162.125.80.6	||ip.addr==143.204.229.69	||ip.addr==162.125.80.5	||ip.addr==162.125.82.1	
