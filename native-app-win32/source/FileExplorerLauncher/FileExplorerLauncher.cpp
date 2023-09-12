/**
 * Copyright (c) 2023, Auckland Transport
 * Computer Vision Team
 * 
 * FileExplorerLauncher is a native Win32 application which will
 * serve as a messaging host to interact with a Chrome/Edge extension.
 * 
 * The app reads a JSON message in the format of {"path":"..."} from
 * stdin as sent by API chrome.runtime.sendNativeMessage (see [1]) following
 * a binary messaging protocol (see [2]).
 * 
 * An instance of Windows File Explorer is then launched in the location
 * specified in the message.
 *
 * References:
 * [1]: https://developer.chrome.com/docs/extensions/reference/runtime/#method-sendNativeMessage
 * [2]: https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/Native_messaging#app_side
 */


#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

// To suppress warnings for freopen
#pragma warning(disable:4996)

/**
 * The class implements the stdio communication with a Chrome/Edge extension.
 * For details see [2].
 */
class MessageBuffer
{
public:
    MessageBuffer() : buff(nullptr) {}
    
    ~MessageBuffer()
    {
        free();
    }

    /**
     * Reads content from a stream (e.g. stdin). 
     */
    std::string read(FILE* stream)
    {
        free();
        size_t bytes = fread(len.buff, 1, sizeof(LengthBuffer), stream);

        if (bytes != sizeof(LengthBuffer)) {
            throw std::exception("ill-formated message from stdin");
        }

        buff = new char[len.bytes + 1];
        buff[len.bytes] = 0;
        bytes = fread(buff, 1, len.bytes, stream);

        return std::string(buff);
    }

    /**
     * Writes content to a stream (e.g. stdout).
     */
    void write(std::string message, FILE* stream)
    {
        free();
        len.bytes = (unsigned long) message.length();
        buff = new char[len.bytes];
        message.copy(buff, len.bytes);

        fwrite(len.buff, 1, sizeof(len), stream);
        fwrite(buff, 1, len.bytes, stream);
        fflush(stream);
    }

    size_t size() const
    {
        return len.bytes;
    }

private:
    /**
     * A message passing stream always starts with a 4-bytes integer.
     * This indicates size of the following message payload.
     */
    union LengthBuffer
    {
        unsigned long bytes;
        unsigned char buff[4];

        LengthBuffer()
        {
            memset(this, 0, sizeof(LengthBuffer));
        }
    };

    LengthBuffer len;
    char* buff; // payload

    /**
     * Frees up the internally allocated buffer.
     */
    void free()
    {
        if (buff != nullptr) {
            delete buff;
            buff = nullptr;
        }

        len.bytes = 0;
    }
} ;

int main(int argc, char *argv[])
{
    // Re-open stdin and stdout in binary mode
    std::freopen(NULL, "rb", stdin);
    std::freopen(NULL, "wb", stdout);

    // Buffer to read/write messages
    MessageBuffer buffer;

    // The prefix and postfix arround file/directory path in 
    // the message payload.
    const std::string prefix = "{\"path\":\"";
    const std::string postfix = "\"}";
    const std::string command = "explorer";

    try {
        std::string message = buffer.read(stdin);
        const int lengthOfPath = message.length() - (prefix.length() + postfix.length());

        if (lengthOfPath <= 0)
        {
            throw std::exception("invalid payload size");
        }

        std::string uncPathToFile = message.substr(prefix.length(), lengthOfPath);
        std::replace(uncPathToFile.begin(), uncPathToFile.end(), '/', '\\');
        
        const std::string launchCommand = command + " " + uncPathToFile;
        system(launchCommand.c_str());

        // For debugging only..
        /**
        std::ofstream log;
        log.open("log.txt");
        log << uncPathToFile << std::endl;
        log.close();
        */
    }
    catch (std::exception ex)
    {
        buffer.write(ex.what(), stdout);
        return -1;
    }

    buffer.write("OKAY", stdout); // signals the extension
    return 0;
}
