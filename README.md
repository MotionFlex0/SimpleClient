# SimpleClient
 Simple Client/Server for testing XOPE. It can run as both a Winsock client and server, with the ability to send messages. This can be used to demonstrate the capture, filter and replay capabilities of XOPE. 

## Usage
 ```SimpleClient.exe MODE PORT```
 ```
 Mode:
  -s  = Run as server
  -c  = Run as client connecting to server
  -cw = Run as client but wait for user input before connecting to server 
  ```
### Example
  Starts a server on port 1337:
  ```
  SimpleClient.exe -s 1337
  ```
  
  Connects to an existing server on port 1337.
  ```
  SimpleClient.exe -c 1337
  ```
  
  Wait for user input before connecting to existing server on port 1337.
  ```
  SimpleClient.exe -cw 1337
  ```
