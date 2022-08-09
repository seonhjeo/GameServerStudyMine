using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using System.Runtime.CompilerServices;
using System.Threading;

/* 로그에 관련된 코드 */
// mainForm.cs로 나오는 창의 맨 밑에 있는 로그박스에 출력시킨다
namespace csharp_test_client
{
    public class DevLog
    {
        // 큐에 로그를 쌓아놓는다
        // mainForm에서 타이머를 이용해 주기적으로 큐에 쌓여있는 로그를 가져와 출력해준다.
        static System.Collections.Concurrent.ConcurrentQueue<string> logMsgQueue = new System.Collections.Concurrent.ConcurrentQueue<string>();

        static Int64 출력가능_로그레벨 = (Int64)LOG_LEVEL.TRACE;

        

        static public void Init(LOG_LEVEL logLevel)
        {
            ChangeLogLevel(logLevel);
        }

        static public void ChangeLogLevel(LOG_LEVEL logLevel)
        {
            Interlocked.Exchange(ref 출력가능_로그레벨, (int)logLevel);
        }

        public static LOG_LEVEL CurrentLogLevel()
        {
            var curLogLevel = (LOG_LEVEL)Interlocked.Read(ref 출력가능_로그레벨);
            return curLogLevel;
        }
        
        // 로그 레벨과 로그 내용을 입력받아 로그 큐에 담아준다.
        static public void Write(string msg, LOG_LEVEL logLevel = LOG_LEVEL.TRACE,
                                [CallerFilePath] string fileName = "",
                                [CallerMemberName] string methodName = "",
                                [CallerLineNumber] int lineNumber = 0)
        {
            if (CurrentLogLevel() <= logLevel)
            {
                logMsgQueue.Enqueue(string.Format("{0}:{1}| {2}", DateTime.Now, methodName, msg));
            }
        }

        static public bool GetLog(out string msg)
        {
            if (logMsgQueue.TryDequeue(out msg))
            {
                return true;
            }

            return false;
        }
        
    }


    public enum LOG_LEVEL
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        DISABLE
    }
}
