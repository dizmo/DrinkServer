#Internet of Drinks Server

This is the server code for the Internet of Drinks Project.


##Hardware

You'll need:

- Arduino Ethernet
- Adafruit Motor Shield V2 (each shield supports 4 pumps)
- Adafruit Peristaltic Pump
- Power Supply 12V

If you want to stack multiple shields, every one except the top one needs stackable headers, while the top one can use the supplied headers.

##Software

The basic implementation of the Internet of Drinks Server is a Websocket Server on Port 80 that receives pump and duration commands.

Every pump has a number between 0 and 7 (in the case of 8 pumps), depending to which port of the Shield it's connected to. Durations (time how long the pump runs) is also specified with a number between 0 and 8. Internally, this number is multiplied by the glass size factor, which then runs the pump for a specified amount of loops. You'll need to adjust the glass size factor according to your needs.

Example:

To have pump 1 dispense the Amount 5, you would send 15 to the Server. You can send more than one pump command at once, e.g 152354, which would run pump 1 for 5 Amounts, pump 2 for 3 and pump 5 for 4.
While one of the pumps is running, you can not send another command to the Server. Once no pump is running anymore, the Server sends the message DONE to the connected client.

Emergency override: To stop all motors at once, send 0919293949596979

A Static IP is advised, otherwise a DHCP Reservation, since the server only discloses its IP Address through the serial console currently.




