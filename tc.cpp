#include <iostream>
#include <termios.h>
#include <unistd.h>

void enableNonCanonicalMode()
{
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int main()
{
    enableNonCanonicalMode();

    char c;
    while (std::cin.get(c))
    {
        std::cout << "Input: " << c << std::endl;
    }

    // Restore the terminal settings before exiting
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);

    return 0;
}
