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
./receiver_&lt;TIMEOUT_METHOD&gt; &lt;save filename&gt; &lt;bind port&gt;  
`Run the receiver on specific port`  
./sender_&lt;TIMEOUT_METHOD&gt; &lt;target address&gt; &lt;connect port&gt;  
`Run the sender on specific port and destination address`  
