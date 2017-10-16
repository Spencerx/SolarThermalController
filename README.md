# Intro

A quick and experimental project for using an arduino to control the electrical backup heater of a solar thermal unit for residential warm water.

# Setup
I have a solar thermal unit on the roof of my home. The electrical resistance (to be used only for backup) is not smart and need some sort of controller.

I'm using an arduino uno with a relais that is able to switch 10A 220V.

I'm also using one 1-Wire temperature sensor inside the hot water tank.

# Algorithm
The idea is to use 2 gates to govern the electrical heater. The premise is that the cooler the water is, the more energy can the solar heater harvest from the sun. So I want to use the electrical heater only if necessary, leaving all the heavy work to the sun.

The first gate is the temperature of the water tank. If it goes blow a certain threshold, then we can continue with the second gate.

The second gate is the time. I don't want the water to be heated at times where I won't be using it anyways, so I prefer to heat it as late as possible. In my case, if I use all hot water during the day and at night it is below the threshold, then it should heat up only between 4am and 6 am, to ensure a minimum temperature for the start of the day. Probably the sun will kick in after 7 am, so no need to heat the water too much with electricity.

# Questions?
Please contact me if you have ideas on how to improve this project. 
