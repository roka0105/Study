using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;

namespace Du3Project
{
    [System.Serializable]
    public class HyperActionCall : UnityEvent<string>
    {

    }


    // 인스팩터에서 보이기위한 클래스
    [System.Serializable]
    public class HyperRegexActionData
    {
        public HyperActionCall CallFN = null;
        public UnityAction<string> CallFN2 = null;
        public Color TextColor = Color.white;

        //"[#＃][가-핳 Ａ-Ｚａ-ｚA-Za-z 0-9０-９]+";
        // "http(s)?://([\w-]+\.)+[\w-]+(/[\w- ./?%&=]*)?" <- 유니티에서 직접 입력시는 \\부분 \ 으로 바꾸기
        public string RegexKey = "http(s)?://([\\w-]+\\.)+[\\w-]+(/[\\w- ./?%&=]*)?";
    }

    public class ConsoleHyperText : BaseConsoleWindow
    {
        public List<HyperRegexActionData> ListRegExActionList = new List<HyperRegexActionData>();
        public UseHyperText m_LinkHyperText = null;

        void InitConsoleHyperText()
        {
            foreach (var item in ListRegExActionList)
            {
                //m_LinkHyperText.OnClick(item.RegexKey, item.CallFN2);
                m_LinkHyperText.OnClick(item.RegexKey
                    , item.TextColor
                    , (p_str) => {
                        Debug.LogFormat("{0}, {1}", item.RegexKey, p_str);
                        item.CallFN.Invoke(p_str);
                    }
                    );
            }
        }

        // 호출 테스트용 함수1
        public void _On_CallFN1(string p_str)
        {
            Debug.LogFormat("CallFN1 : {0}", p_str);
        }
        // 호출 테스트용 함수2
        public void _On_CallFN2(string p_str)
        {
            Debug.LogFormat("CallFN2 : {0}", p_str);

        }


        private void Awake()
        {
            InitConsoleHyperText();
            InitSetting();

        }


        void Start()
        {

        }

        void Update()
        {
            UIUpdateConsole();
        }


    }
}