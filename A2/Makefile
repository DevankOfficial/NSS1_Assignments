vm2:
  openssl genpkey -algorithm RSA -out VM2_key.pem
  openssl req -new -key VM2_key.pem -out vm2_csr.pem
  openssl x509 -req -in vm2_csr.pem -signkey your_key.pem -out vm2_cert.pem
  openssl x509 -in vm2_cert.pem -text -noout

vm3:
  openssl genpkey -algorithm RSA -out VM3_key.pem
  openssl req -new -key VM3_key.pem -out vm3_csr.pem
  openssl x509 -req -in vm3_csr.pem -signkey VM3_key.pem -out vm3_cert.pem
  openssl x509 -in vm3_cert.pem -text -noout

cap:
  tcpdump -i eth0 host 192.168.0.108 and esp -nn
