#include <drogon/drogon.h>
#include <csignal>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>

static pid_t workerPid = 0;
const char *pidFile = "/tmp/drogon_master.pid";
const char *logFile = "/tmp/drogon_app.log";

// Forward declarations
void startWorker();
void stopWorker();
void restartWorker();
void masterSignalHandler(int sig);
void writePidFile();
pid_t readMasterPid();
bool isProcessRunning(pid_t pid);
void runMaster();

void startWorker()
{
    workerPid = fork();
    if (workerPid == 0)
    {
        // Child: redirect stdout/stderr to log file
        int fd = open(logFile, O_CREAT | O_WRONLY | O_APPEND, 0644);
        if (fd != -1)
        {
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
        // Run Drogon app
        drogon::app().loadConfigFile("./config.yaml");
        drogon::app().run();
        _exit(0);
    }
    else if (workerPid > 0)
    {
        std::cout << "Started worker PID: " << workerPid << std::endl;
    }
    else
    {
        std::cerr << "Failed to fork worker\n";
    }
}

void stopWorker()
{
    if (workerPid > 0)
    {
        std::cout << "Stopping worker PID: " << workerPid << std::endl;
        kill(workerPid, SIGINT);
        int status = 0;
        waitpid(workerPid, &status, 0);
        std::cout << "Worker stopped.\n";
        workerPid = 0;
    }
}

void restartWorker()
{
    std::cout << "Restarting worker...\n";
    stopWorker();
    startWorker();
}

void masterSignalHandler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM)
    {
        std::cout << "Master received signal to quit.\n";
        stopWorker();
        unlink(pidFile);
        exit(0);
    }
    else if (sig == SIGUSR1)
    {
        restartWorker();
    }
}

void writePidFile()
{
    std::ofstream ofs(pidFile);
    if (ofs)
    {
        ofs << getpid();
        ofs.close();
    }
    else
    {
        std::cerr << "Failed to write PID file\n";
    }
}

pid_t readMasterPid()
{
    std::ifstream ifs(pidFile);
    if (ifs)
    {
        pid_t pid;
        ifs >> pid;
        return pid;
    }
    return 0;
}

bool isProcessRunning(pid_t pid)
{
    if (pid <= 0)
        return false;
    // Check if process exists by sending signal 0
    return (kill(pid, 0) == 0);
}

void tailLogs()
{
    std::cout << "Tailing log file: " << logFile << "\nPress Ctrl+C to stop.\n";
    std::string cmd = "tail -f ";
    cmd += logFile;
    system(cmd.c_str());
}

void runMaster()
{
    writePidFile();

    std::signal(SIGINT, masterSignalHandler);
    std::signal(SIGTERM, masterSignalHandler);
    std::signal(SIGUSR1, masterSignalHandler);

    startWorker();

    while (true)
    {
        pause();
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        std::string cmd = argv[1];

        if (cmd == "start")
        {
            pid_t masterPid = readMasterPid();
            if (isProcessRunning(masterPid))
            {
                std::cout << "Master already running with PID " << masterPid << std::endl;
                return 0;
            }
            else
            {
                std::cout << "Starting master process...\n";
                runMaster();
                return 0;
            }
        }
        else if (cmd == "stop")
        {
            pid_t masterPid = readMasterPid();
            if (isProcessRunning(masterPid))
            {
                std::cout << "Stopping master PID " << masterPid << std::endl;
                kill(masterPid, SIGTERM);
            }
            else
            {
                std::cout << "Master not running.\n";
            }
            return 0;
        }
        else if (cmd == "restart")
        {
            pid_t masterPid = readMasterPid();
            if (isProcessRunning(masterPid))
            {
                std::cout << "Sending restart signal to master PID " << masterPid << std::endl;
                kill(masterPid, SIGUSR1);
            }
            else
            {
                std::cerr << "Master PID not found or not running. Start the app first.\n";
            }
            return 0;
        }
        else if (cmd == "log" && argc > 2 && std::string(argv[2]) == "-f")
        {
            tailLogs();
            return 0;
        }
        else
        {
            std::cerr << "Unknown command\n";
            return 1;
        }
    }

    // Default: run master if no command
    runMaster();

    return 0;
}
