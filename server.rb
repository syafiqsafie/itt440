require 'socket'

server1 = TCPServer.open(1030)
loop{
	client = server1.accept
	client.puts(Time.now.ctime)
	client.puts "CONNECTION CLOSE. BYE!"
	client.close
}
