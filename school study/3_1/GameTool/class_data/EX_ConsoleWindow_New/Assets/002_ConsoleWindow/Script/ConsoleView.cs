using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.Text;
using UnityEngine.EventSystems;

namespace Du3Project
{
    public class ConsoleView : MonoBehaviour
    {
        ConsoleController console = new ConsoleController();

        bool didShow = false;

        public GameObject viewContainer; //콘솔뷰용 컨테이너값
        public Text logTextArea;
        public InputField inputField;

        // "`" 키값
        public KeyCode ConsoleViewKeyCode = KeyCode.BackQuote;

        void Start()
        {
            if (console != null)
            {
                console.visibilityChanged += onVisibilityChanged;
                console.logChanged += onLogChanged;
            }
            updateLogStr(console.log);

            setVisibility(false);
        }

        ~ConsoleView()
        {
            console.visibilityChanged -= onVisibilityChanged;
            console.logChanged -= onLogChanged;
        }

        void Update()
        {
            // 콘솔띄우는 키보드 누르면 화면 나오도록 하기
            if (Input.GetKeyUp(ConsoleViewKeyCode))
            {
                toggleVisibility();
            }

            // 터치이벤트로 손가락 5개 선택하면 디버그창 나오도록 하기
            if (Input.touches.Length == 5)
            {
                if (!didShow)
                {
                    toggleVisibility();
                    didShow = true;
                }
            }
            else
            {
                didShow = false;
            }
        }

        void toggleVisibility()
        {
            setVisibility(!viewContainer.activeSelf);

            if (false)
            {
                // 기본 inputfield 선택 방법
                inputField.Select();
                inputField.ActivateInputField();
            }
            else
            {
                // 모든것 선택하는 방식
                EventSystem.current.SetSelectedGameObject(inputField.gameObject, null);
                inputField.OnPointerClick(new PointerEventData(EventSystem.current));
            }

        }

        void setVisibility(bool visible)
        {
            viewContainer.SetActive(visible);
        }

        void onVisibilityChanged(bool visible)
        {
            setVisibility(visible);
        }

        void onLogChanged(string[] newLog)
        {
            updateLogStr(newLog);
        }

        void updateLogStr(string[] newLog)
        {
            if (newLog == null)
            {
                logTextArea.text = "";
            }
            else
            {
                logTextArea.text = string.Join("\n", newLog);
            }
        }

        /// 
        /// 버턴클릭시 함수 연결해서 호출하기위한 함수
        /// 
        public void _On_RunCommand()
        {
            // echo "abc efg help" -> echo 에 abc, efg, help 란 arge값으로 파싱을 해줌
            console.runCommandString(inputField.text);
            inputField.text = "";
        }

    }
}


