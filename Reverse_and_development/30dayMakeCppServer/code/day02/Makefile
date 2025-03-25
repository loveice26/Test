# 定义目标：server
# 该目标用于编译生成客户端和服务器可执行文件
server:
	# 编译客户端程序，将 util.cpp 和 client.cpp 编译为 client 可执行文件
	g++ util.cpp client.cpp -o client && \
	# 编译服务器程序，将 util.cpp 和 server.cpp 编译为 server 可执行文件
	g++ util.cpp server.cpp -o server
# 定义目标：clean
# 该目标用于清理编译生成的可执行文件
clean:
	# 删除 server 可执行文件
	rm server && \
	# 删除 client 可执行文件
	rm client
