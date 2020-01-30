# UDP-File-Transfer  
This is a pratice project of Intro. to Network Programming.
====================================
***Recommended: If you are taking this course, please solve it on yourself first.***  

This is a UDP file transfer with three implements of timeout methods.  

Timeout using sigalarm  
Timeout using select  
Timeout using setsockopt  

./make all  
`Compile all client and server programs`  
./receiver_[TIMEOUT_METHOD] [save filename] [bind port]  
`Run the receiver on specific port`  
./sender_[TIMEOUT_METHOD] [target address] [connect port]  
`Run the sender on specific port and destination address`  
