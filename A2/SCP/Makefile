scp:
	rm -f scp_ct scp_sr
	g++ -o scp_ct scp_client.cc ../Headers/encryption_util.cc -lssl -lcrypto
	g++ -o scp_sr scp_server.cc ../Headers/encryption_util.cc -lssl -lcrypto

clean:
	rm -f scp_ct scp_sr