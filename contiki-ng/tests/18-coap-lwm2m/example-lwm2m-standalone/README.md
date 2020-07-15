# Example LWM2M Standalone

An example of how to use the Contiki-NG's LWM2M stack on native posix without
dependencies on having the whole Contiki-NG source tree involved. This is
designed to illustrate how to make use of the LWM2M client code if using
another OS, or if not using a regular IP-stack for the communication.

# Building

To build the example just go to the lwm2m folder and do make.

# Running

If you just would like to try the client out with the already available leshan
server you can just do:

     ./lwm2m-example

and it will connect to that server.

Browse to the server via http://leshan.eclipse.org to check if the client
registered and to read out values from it.

# Running with another LWM2M server

You can specify another LWM2M server when running:

     ./lwm2m-example coap://<IP address of LWM2M server>

and it will connect to the specified server.
