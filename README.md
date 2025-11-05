# SodiumBulb
low pressure sodium vapor bulb
This is designed to drive a GP20NA or GP20ND low pressure sodium vapor bulb. 

These bulbs normally run on 15-20 volts at 1-2 amps. However, ignition is much more difficult. They require an ignition pulse of >400 volts with oomph behind it for a second or two in order to jump the gap between the main electrodes. This made designing a compact battery powered driver very difficult.

There is only one step up transformer in the circuit that serves dual purposes. During normal operation the secondary is disconnected and the primary is used as an inductor in a boost converter to generate the 15-20 volts to power the lamp. However during ignition, relays connect the secondary of the transformer to the bulb, and then the primary is driven HARD to get a discharge going across the electrode gap. The transformer and mosfet will overheat rapidly during this, but it only takes a couple seconds. It's also quite fun, as it gives you the feeling of igniting a flame lantern.


The transformer was salvaged from a power supply, should be about a 10x step up. A transformer from one of those chinese 12V to 350V DC-DC boost converter boards should work. You will have to experiment with bulb polarity when hooking it to the circuit, Mine ignited much easier one way vs the other.

The 12 volt power in doesn't have its own connector, it is connected to pins 1(gnd) and 2(12v) on the GPG terminal block on the power board. Study the schematic to confirm.

Bulb: https://www.aliexpress.us/item/2255801048097192.html


