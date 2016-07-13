require "rubygems"

require "nokogiri"
require "open-uri"
require "serialport"

doc = Nokogiri::HTML(open("https://cc.buildbox.io/ferocia.xml?api_key=36a28a2f2d9c3758d9059830143ecaef&branch=master"))
build = doc.xpath(%q(//projects/project[@name="johanna (master)"])).first

dev = "/dev/tty.usbmodemfa131"
baud = 9600

i = case [build["activity"], build["lastbuildstatus"]]
    when ["Sleeping", "Failure"] then 0
    when ["Sleeping", "Success"] then 1
    when ["Building", "Failure"] then 2
    when ["Building", "Success"] then 3
    end

serial_port = SerialPort.new(dev, baud)
serial_port.write "#{i}\n"
serial_port.close
