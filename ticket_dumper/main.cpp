#include <iostream>
#include <Windows.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>

// Steamworks SDK
#include <steam_api.h>

namespace fs = std::filesystem;

// Check if the argument is a numeric value
inline bool is_numeric( const std::string& arg )
{
	return !arg.empty() && std::all_of( arg.begin(), arg.end(), ::isdigit );
}

int main( int argc, char* argv[] )
{
	if (argc < 2)
	{
		std::cout << "Usage: ticket_dumper <steam_app_id>" << std::endl;
		return -1;
	}

	// The argument is not a numeric value
	if (!is_numeric( argv[1] ))
	{
		std::cout << "[ERROR]" << ' ' << "The argument is not a numeric  value." << std::endl;
		return -1;
	}

	auto set_steam_var = SetEnvironmentVariableA( "SteamAppId", argv[1] );

	// Failed to set the 'SteamAppId' environment variable
	if (!set_steam_var)
	{
		std::cout << "[ERROR]" << ' ' << "Failed to set the 'SteamAppId' environment variable." << std::endl;
		return -1;
	}

	auto init = SteamAPI_Init();

	// Failed to initialize Steamworks
	if (!init)
	{
		std::cout << "[ERROR]" << ' ' << "Failed to initialize Steamworks." << std::endl;
		return -1;
	}

	auto logged = SteamUser()->BLoggedOn();

	// Steam user is not logged on
	if (!logged)
	{
		std::cout << "[ERROR]" << ' ' << "Steam user is not logged on." << std::endl;
		return -1;
	}

	UCHAR ticket[1024] {};
	uint32 size { 0 };

	auto get_ticket = SteamUser()->GetAuthSessionTicket( &ticket, 1024, &size );

	// The call has returned 'k_HAuthTicketInvalid'
	if (get_ticket == 0)
	{
		std::cout << "[ERROR]" << ' ' << "The call has returned 'k_HAuthTicketInvalid'." << std::endl;
		return -1;
	}

	std::fstream file {};
	auto dumped_ticket = fs::path( argv[1] ) += ".bin";

	file.open( dumped_ticket.generic_wstring(), std::ios::out | std::ios::binary );
	file.write( reinterpret_cast<char*>(&ticket), size );
	file.close();

	return 0;
}