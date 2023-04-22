#include "Main.h"
#include "LauncherAPI.h"

int main()
{
    LauncherAPI::MessageBox::Show("Test message box", "Test message!", LauncherAPI::MessageBoxButtons::OK,
        [](int button) 
        {
            if (button == LauncherAPI::API_OK)
            {
                std::cout << "clicked OK";
            }
            else if (button == LauncherAPI::API_UNKNOWN)
            {
                std::cout << "cancelled";
            }
        }
    );
}