namespace Hanabi
{
    internal enum LogLevel
    {
        Trace = 1 << 0,
        Info = 1 << 1,
        Warn = 1 << 2,
        Error = 1 << 3,
        Critical = 1 << 4,
    }

    public class Log
    {
        public static void Trace(string message)
        {
            InternalCalls.Log_LogMessage(LogLevel.Trace, message);
        }

        public static void Info(string message)
        {
            InternalCalls.Log_LogMessage(LogLevel.Info, message);
        }

        public static void Warn(string message)
        {
            InternalCalls.Log_LogMessage(LogLevel.Warn, message);
        }

        public static void Error(string message)
        {
            InternalCalls.Log_LogMessage(LogLevel.Error, message);
        }

        public static void Critical(string message)
        {
            InternalCalls.Log_LogMessage(LogLevel.Critical, message);
        }
    }
}
