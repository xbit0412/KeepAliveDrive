#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>
#include <windows.h>
#include <cstdlib>
#include <string>

int main()
{
    // Asignamos a una variable el %localappdata% del usuario en ejecucion
    std::string userappdataLocation = std::getenv("LOCALAPPDATA");
    // Asignamos en una variable el fichero KeepAliveDrive.txt manteniendo la ruta %localappdata% anterior
    std::string driveconfig = userappdataLocation + "\\KeepAliveDrive.txt";

    // Gestionamos la seleccion de la unidad donde esta la microsd
    // Si el archivo driveconfig NO existe o el fichero esta vacio, ejecutamos el asistente de seleccion de unidad microsd que crea el fichero driveconfig
    if (!std::filesystem::exists(driveconfig) || std::filesystem::is_empty(driveconfig))
    {
        std::string driveSelection;
        std::cout << "Enter microsd drive letter mount (for example D): ";
        std::cin >> driveSelection;
        std::ofstream output(driveconfig);
        output << driveSelection + ":";
        // Liberamos al programa de la linea de comandos para que siga ejecutandose de forma transparente
        FreeConsole();
    }

    // Verificamos que existe el fichero con la unidad de la microsd en %localappdata%
    if (!std::filesystem::exists(driveconfig) || std::filesystem::is_empty(driveconfig))
        {
            // Si no existe el fichero o esta vacio, mostramos error, como solucionar, y cerramos programa
            std::cout << "Error: File " << userappdataLocation << "\\driveconfig.txt is not valid" << std::endl;
            std::cout << "To fix it, insert a letter where your device is mounted, for example D" << std::endl;
            return 0;
        }

    else
        {   // Si el fichero existe configurado ok, hacemos un bucle en el que cada 3 segundos escriba en el fichero keepalive.txt el caracter 0 y 1, previniendo que la unidad se duerma y genere los stutters de reactivacion
            // Seleccionamos el fichero %localappdata%\KeepAliveDrive.txt para reutilizar la variable global driveconfig
            std::string driveSelection;
            std::ifstream input(driveconfig);
            input >> driveSelection;

            // Creamos el fichero con el valor 0
             HANDLE hFile = CreateFileA(
                        (driveSelection + "\\keepalive.txt").c_str(),
                        GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        nullptr,
                        OPEN_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        nullptr
                    );

                    char value = '0';
                    DWORD written;

                    SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
                    WriteFile(hFile, &value, 1, &written, nullptr);

                    CloseHandle(hFile);

             // Creamos el fichero como atributo hidden para que el usuario no lo vea cada que abrimos la unidad
            SetFileAttributesA((driveSelection + "\\keepalive.txt").c_str(), FILE_ATTRIBUTE_HIDDEN);
            // Liberamos al programa de la linea de comandos para que siga ejecutandose de forma transparente
            FreeConsole();

            // Ejecutamos el blucle de escritura
            while (true)
                {   
                    // Loop del escribir 0
                    HANDLE hFile = CreateFileA(
                        (driveSelection + "\\keepalive.txt").c_str(),
                        GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        nullptr,
                        OPEN_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        nullptr
                    );

                    char value = '0';
                    DWORD written;

                    SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
                    WriteFile(hFile, &value, 1, &written, nullptr);

                    CloseHandle(hFile);

                    std::this_thread::sleep_for(std::chrono::seconds(3));

                    // LOOp de escribir 1
                    hFile = CreateFileA(
                        (driveSelection + "\\keepalive.txt").c_str(),
                        GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        nullptr,
                        OPEN_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        nullptr
                    );

                    value = '1';

                    SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
                    WriteFile(hFile, &value, 1, &written, nullptr);

                    CloseHandle(hFile);

                    std::this_thread::sleep_for(std::chrono::seconds(3));
                }

                }

    return 0;
}
