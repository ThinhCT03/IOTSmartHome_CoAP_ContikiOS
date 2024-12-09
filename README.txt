*ABOUT THE PROJECT*:
This basic IOT SmartHome model is built including sensor nodes that collect information, a border router node plays the role of communicating with the external internet network and using extensions (add-on) Copper for Mozilla Firefox browser is designed to interact with The device uses the CoAP protocol through the border router - acting as a node client server with which users can interact and control nodes in the home network intelligently through commands on Copper such as: GET, POST, PUT and DELETE.


-----------------------------------------------STEPS THAT SHOW HOW WE CAN RUN THIS PROJECT ON CONTIKI-OS--------------------------------------------------------------

1. Make sure you have your computer installed with Contiki OS (not Contiki NG, but technically, it would work too).
2. Power the Contiki --> Home --> Contiki --> Examples --> er-rest-example. Clone this repo to the er-rest-example directory (noted that you have to delete the old MAKEFILE before clone the new one in).
3. Run Cooja --> New simulation --> Add 1 mote from file border-router.c in this link: examples/ipv6/rpl-border-router (Make sure that you add this node first).
4. Add all the node you cloned from this repo --> open Serial Socker (Server) in the border router --> click Start --> Open terminal, then type "tunslipcooja", type password --> hit RUN simulation --> Complete. (If you don't have tunslipcooja - a standalone utility program, then you can follow this instruction on youtube to run the simulation: https://www.youtube.com/watch?v=eRQkgAKdiEk&t=838s).
5. One big note: the node CO_Fan_client did not work probably on simulation right now, so dont add it in. It will be update the right version later.

--------------------------------------------------------------COMMENT ABOUT THE MINIPROJECT---------------------------------------------------------------------------

ON THE POSITIVE SIDE: THE SYSTEM HAS WORKED AS THE GROUP'S GOALS AND HAS SIMULATED A SMART HOME IOT APPLICATION USING COAP WITH FULL FUNCTION.

ON THE UNSUCCESSFUL ASPECTS: A REAL USER INTERFACE HAS NOT BEEN BUILT, AS WELL AS THE WEAKNESSES OF COPPER WHEN USED TO VERIFY OPERATION AND CANNOT BE DEPLOYED ON REAL HARDWARE.
