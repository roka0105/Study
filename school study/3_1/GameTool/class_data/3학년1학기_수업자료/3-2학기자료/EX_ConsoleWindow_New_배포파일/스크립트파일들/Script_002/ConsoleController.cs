using System;
using System.Text;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

namespace Du3Project
{
    public delegate void CommandHandler(string[] args);

    public class ConsoleController
    {

        #region 이벤트용들
        // Used to communicate with ConsoleView
        public delegate void LogChangedHandler(string[] log);
        public event LogChangedHandler logChanged;

        public delegate void VisibilityChangedHandler(bool visible);
        public event VisibilityChangedHandler visibilityChanged;
        #endregion

        /// 
        /// Object to hold information about each command
        /// 
        class CommandRegistration
        {
            public string command { get; private set; }
            public CommandHandler handler { get; private set; }
            public string help { get; private set; }

            public CommandRegistration(string command, CommandHandler handler, string help)
            {
                this.command = command;
                this.handler = handler;
                this.help = help;
            }
        }



        /// 
        /// 스크롤에 사용할 문자라인 사이즈값
        /// 
        const int scrollbackSize = 20;

        Queue<string> scrollback = new Queue<string>(scrollbackSize);

        List<string> commandHistory = new List<string>();
        Dictionary<string, CommandRegistration> commands = new Dictionary<string, CommandRegistration>();

        public string[] log { get; private set; } // 로그에 남길 문자배열값

        const string repeatCmdName = "!!"; //커맨드 명령 알수 있는 명령어값 현재는 사용하고 있지않음 추가적인작업하면됨

        public ConsoleController()
        {
            // 커맨드 등록용 
            // 해당 명령어들의 각각 파라매터 값들은 echo "foo bar baz" 하면 파라매터로 [foo, bar, baz] 형태로 잘라서 파라매터로 보내준다
            registerCommand("echo", echo, "echoes arguments back as array (for testing argument parser)");
            registerCommand("help", help, "Print this help.");
            registerCommand("hide", hide, "Hide the console.");
            registerCommand("repeatCmdName", repeatCommand, "Repeat last command.");
            registerCommand("reload", reload, "Reload game.");
            registerCommand("resetprefs", resetPrefs, "Reset & saves PlayerPrefs.");

            registerCommand("ImageColor", ImageColorChange, "0~ 1 사이값 ImageColor [itemname 1 0 1]");
        }


        void registerCommand(string command, CommandHandler handler, string help)
        {
            commands.Add(command.ToLower(), new CommandRegistration(command, handler, help));
        }

        public void appendLogLine(string line)
        {
            Debug.Log(line);

            if (scrollback.Count >= ConsoleController.scrollbackSize)
            {
                scrollback.Dequeue();
            }
            scrollback.Enqueue(line);

            log = scrollback.ToArray();
            if (logChanged != null)
            {
                logChanged(log);
            }
        }

        /// <summary>
        /// commandString 로 입력된 문자값을 파싱하고 
        /// 해당 커맨드가 있으면 해당 커맨드와 같이 연결이된 함수를 호출 하는 방식
        /// </summary>
        /// <param name="commandString"></param>
        public void runCommandString(string commandString)
        {
            appendLogLine("$ " + commandString);

            string[] commandSplit = parseArguments(commandString);
            string[] args = new string[0];
            if (commandSplit.Length >= commandString.Length)
            {
                return;
            }
            else if (commandSplit.Length >= 2)
            {
                int numArgs = commandSplit.Length - 1;
                args = new string[numArgs];
                Array.Copy(commandSplit, 1, args, 0, numArgs);
            }
            runCommand(commandSplit[0].ToLower(), args);
            commandHistory.Add(commandString);
        }

        public void runCommand(string command, string[] args)
        {
            CommandRegistration reg = null;
            // command 로 등록된 문자열을 찾아서 reg 등록 하도록 하기
            if (!commands.TryGetValue(command, out reg))
            {
                appendLogLine(string.Format("등록된 명령어가 없습니다 : '{0}', 명령어 -> 'help' 를치면 명령어 리스트가 나옵니다.", command));
            }
            else
            {
                // delegator 가 있으면 콜백 함수 호출 하도록 처리
                if (reg.handler == null)
                {
                    appendLogLine(string.Format("등록된 명령어에는 연결된 함수가 없습니다 : '{0}'.", command));
                }
                else
                {
                    reg.handler(args);
                }
            }
        }

        /// <summary>
        /// echo "aa bb cc" 명령어에서 "aa bb cc" 안의 문자만 ' ' 스페이스값 으로 파라매터별로 나누도록 처리됨
        /// regex등을 이용해서 파싱 구문을 만들어더 좋음
        /// </summary>
        /// <param name="commandString"></param>
        /// <returns></returns>
        static string[] parseArguments(string commandString)
        {
            LinkedList<char> parmChars = new LinkedList<char>(commandString.ToCharArray());
            bool inQuote = false;
            var node = parmChars.First;
            while (node != null)
            {
                var next = node.Next;
                if (node.Value == '"')
                {
                    inQuote = !inQuote;
                    parmChars.Remove(node);
                }
                if (!inQuote && node.Value == ' ')
                {
                    node.Value = '\n';
                }
                node = next;
            }
            char[] parmCharsArr = new char[parmChars.Count];
            parmChars.CopyTo(parmCharsArr, 0);
            return (new string(parmCharsArr)).Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
        }

        #region Command handlers

        // string[] args 파라매터값은 무조건 배열로 날아옴
        void ImageColorChange(string[] args)
        {
            if (args.Length != 4)
            {
                return;
            }

            GameObject obj = GameObject.Find(args[0]);
            if (obj == null)
                return;

            Image img = obj.GetComponent<Image>();
            if (img != null)
            {
                float r = float.Parse(args[1]);
                float g = float.Parse(args[2]);
                float b = float.Parse(args[3]);
                Color changecolor = new Color(r, g, b);
                img.color = changecolor;
            }
        }

        void help(string[] args)
        {
            appendLogLine(">> 명령어들");
            foreach (var item in commands)
            {
                string tempstr = string.Format("\t {0} : {1}", item.Key, item.Value.help);
                appendLogLine(tempstr);
                //Debug.LogFormat("Help Command : ", item.Key);
            }
        }
        void hide(string[] args)
        {
            if (args.Length > 0)
                Debug.LogFormat("Hide Command: {0}", args);
            else
                Debug.LogFormat("Hide Command: ");

            if (visibilityChanged != null)
                visibilityChanged.Invoke(false);
        }

        // 테스트용 값
        void echo(string[] args)
        {
            if (args.Length > 0)
                Debug.LogFormat("Echo Command: {0}", args);
            else
                Debug.LogFormat("Echo Command: ");
        }

        void repeatCommand(string[] args)
        {
            if (args.Length > 0)
                Debug.LogFormat("repeat Command: {0}", args);
            else
                Debug.LogFormat("repeat Command: ");
        }

        void reload(string[] args)
        {
            // 강제주석
            //UnityEngine.SceneManagement.Scene currscene = UnityEngine.SceneManagement.SceneManager.GetActiveScene();
            //UnityEngine.SceneManagement.SceneManager.LoadScene(currscene.buildIndex );
        }

        void resetPrefs(string[] args)
        {
            PlayerPrefs.DeleteAll();
            PlayerPrefs.Save();
        }

        #endregion
    }
}





