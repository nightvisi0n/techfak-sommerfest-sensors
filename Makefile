.PHONY: all dht22 upload_dht22

all: dht22

dht22:
	pio run -d dht22

upload_dht22: dht22
	pio run -d dht22 -t upload
