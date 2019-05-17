namespace logging {
    void log(string message) {
        time_t t = time(0);
        struct tm* tmp = localtime(&t);
        char time_string[50];
        strftime(time_string, 50, "[%x - %I:%M:%S] ", tmp);
        cout << time_string << message << endl;
    }
}
