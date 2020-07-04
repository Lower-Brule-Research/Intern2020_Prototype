import serial
import requests

arduino = serial.Serial('/dev/cu.usbserial-1410', 9600, timeout=.1)
while True:
	data = arduino.readline()[:-2] #the last bit gets rid of the new-line chars
	if data:
		print(data)
		logURL = "https://script.google.com/macros/s/AKfycbzmBuyB3H7nwHbfPpSntC_cdKOfRfp1JRZvo4OAKe-IiAcGoCg/exec"
		URLa = "?temperature="
		URLb = str(data)
		URLc = "&humidity="
		URLd = ""
		URL = logURL+URLa+URLb+URLc+URLd
		r = requests.get(URL)
