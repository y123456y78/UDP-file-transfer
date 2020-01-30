make:
	gcc -o receiver_sockopt receiver_sockopt.c
	gcc -o sender_sockopt sender_sockopt.c
	gcc -o receiver_select receiver_select.c
	gcc -o sender_select sender_select.c
	gcc -o receiver_sigalrm receiver_sigalrm.c
	gcc -o sender_sigalrm sender_sigalrm.c
clean:
	rm -f receiver_sockopt sender_sockopt receiver_select sender_select receiver_sigalrm sender_sigalrm test_in test_sockopt test_select test_sigalrm
