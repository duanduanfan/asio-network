#include "Session.h"

void Session::Start()
{
	memset(_data, 0, max_length);
	_socket.async_read_some(boost::asio::buffer(_data, max_length),
		std::bind(&Session::handle_read, this,
			std::placeholders::_1, std::placeholders::_2)
	);
}

void Session::handle_read(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if (error) {
		std::cout << "read error" << std::endl;
		delete this;
		return;
	}

	std::cout << "server receive data is " << _data << std::endl;
	boost::asio::async_write(_socket, boost::asio::buffer(_data, bytes_transferred),
		std::bind(&Session::handle_write, this, std::placeholders::_1)
	);
}

void Session::handle_write(const boost::system::error_code& error)
{
	if (error) {
		std::cout << "read error" << std::endl;
		delete this;
		return;
	}
	memset(_data, 0, max_length);
	_socket.async_read_some(boost::asio::buffer(_data, max_length),
		std::bind(&Session::handle_read, this,
			std::placeholders::_1, std::placeholders::_2)
	);
}

Server::Server(boost::asio::io_context& ioc, short port)
	:_ioc(ioc), _acceptor(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
	std::cout << "Server start success, on port: " << port << std::endl;
	start_accept();
}

void Server::start_accept()
{
	Session* new_session = new Session(_ioc);
	_acceptor.async_accept(new_session->Socket(), std::bind(&Server::handle_accept,
		this, new_session, std::placeholders::_1)
	);
}

void Server::handle_accept(Session* new_session, const boost::system::error_code& error)
{
	if (error) {
		delete new_session;
	}
	else {
		new_session->Start();
	}
	start_accept();

}
