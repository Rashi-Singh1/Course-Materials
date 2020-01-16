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




# Question 3 : 




# Question 4 : 

	Notes on ifconfig :
		stands for I nter F ace CONFIG uration 


# Question 5 : 




# Question 6 : 




# Question 7 : 



# Question 8 : 