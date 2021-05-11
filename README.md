# Arduino Irrigation Controller

TODO: Delete this and the text above, and describe your project

## Installation

###### Prerequisites
- Some linuxy environment (WSL, Ubuntu, etc)
- Platformio  

Download this repo with git and install the platform.

```
$ sudo apt-get install -y build-essential git
$ git clone https://github.com/TheNotary/arduino-irrigation-controller
$ cd arduino-irrigation-controller
$ pio platform install atmelavr native
$ pio lib install
```

Connect your Arduino Mega and flash it using platformio:

    $ platformio run

## Test

    $ pio test -e native


## Parts

* [TODO:  Add Parts](http://127.0.0.1)

## Wiring

TODO: Drop a link to a picture of your wiring here or something.  Maybe fritzing.

## Device Operation

TODO: Explain how to use the device here


## Contributing

1. Fork it ( https://github.com/[my-github-username]/arduino-irrigation-controller/fork )
2. Create your feature branch (`git checkout -b my-new-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin my-new-feature`)
5. Create a new Pull Request
