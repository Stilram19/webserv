# include <unistd.h>
# include <iostream>
# include <fcntl.h>

int	main()
{
	if (access("/Users/obednaou/tests/file", F_OK))
		std::cout << "Invalid" << std::endl;
	else
		std::cout << "Valid!" << std::endl;
}
