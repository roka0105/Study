using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

namespace Du3Project
{
    public class BaseConsoleWindow : MonoBehaviour
    {
        [Header("[베이스콘솔 기본 설정용]")]
        public GameObject viewContainer; //Container for console view, should be a child of this GameObject
        public Text logTextArea;
        public InputField inputField;
        public Button InputButton;
        public Button ClearButton;

        protected ScrollRect m_ChildTextScrollRect = null;

        // "`" 키값
        public KeyCode ConsoleViewKeyCode = KeyCode.BackQuote;

        protected bool m_DidShow = false;


        protected StringBuilder m_ConsolString = new StringBuilder();
        const string LINE_SPLIT_RE = @"\r\n|\n\r|\n|\r";


        protected virtual void CallInputFieldEndConsoleAddLabel(string p_str)
        {

        }

        protected virtual void AppendText(string p_str)
        {
            m_ConsolString.Append("\n").Append(p_str);

            logTextArea.text = m_ConsolString.ToString();
        }

        protected virtual void UIUpdateScrollbar()
        {
            Canvas.ForceUpdateCanvases();
            m_ChildTextScrollRect.verticalNormalizedPosition = 0f;
        }
        public virtual void _On_InputFieldEndEvent(string p_str)
        {
            if (inputField.text.Length > 0
                && (Input.GetKeyDown(KeyCode.KeypadEnter) || Input.GetKeyDown(KeyCode.Return))
                )
            {
                string tempstr = Regex.Replace(inputField.text, LINE_SPLIT_RE, "");
                AppendText(tempstr);
                inputField.text = "";

                SetFocusInputField();
                UIUpdateScrollbar();

                // 명령어 함수 호출하기 위한 방법
                CallInputFieldEndConsoleAddLabel(tempstr);
            }
        }

        protected void SetFocusInputField()
        {
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

                inputField.caretPosition = inputField.text.Length;
            }
        }

        protected void toggleVisibility()
        {
            setVisibility(!viewContainer.activeSelf);

            if (viewContainer.activeSelf)
            {
                SetFocusInputField();
            }
        }
        protected void setVisibility(bool visible)
        {
            viewContainer.SetActive(visible);
        }

        protected virtual void UIUpdateConsole()
        {
            if (Input.GetKeyUp(ConsoleViewKeyCode))
            {
                toggleVisibility();
            }

            // 터치이벤트로 손가락 5개 선택하면 디버그창 나오도록 하기
            if (Input.touches.Length == 5)
            {
                if (!m_DidShow)
                {
                    toggleVisibility();
                    m_DidShow = true;
                }
            }
            else
            {
                m_DidShow = false;
            }
        }

        public virtual void _On_InputBTNClick()
        {
            string tempstr = Regex.Replace(inputField.text, LINE_SPLIT_RE, "");
            AppendText(tempstr);
            inputField.text = "";

            SetFocusInputField();
            UIUpdateScrollbar();

            // 명령어 함수 호출하기 위한 방법
            CallInputFieldEndConsoleAddLabel(tempstr);
        }

        public virtual void _On_ClearMessageBTNClick()
        {
            m_ConsolString.Length = 0;
            logTextArea.text = "";
        }

        protected virtual void InitSetting()
        {
            m_ChildTextScrollRect = GetComponentInChildren<ScrollRect>();

            inputField.onEndEdit.AddListener(_On_InputFieldEndEvent);
            InputButton.onClick.AddListener(_On_InputBTNClick);
            if (ClearButton)
                ClearButton.onClick.AddListener(_On_ClearMessageBTNClick);

            logTextArea.text = "";
            m_ConsolString.Length = 0;
            setVisibility(false);

        }

        private void Awake()
        {
            InitSetting();


        }

        ~BaseConsoleWindow()
        {
            inputField.onEndEdit.RemoveListener(_On_InputFieldEndEvent);
            InputButton.onClick.RemoveListener(_On_InputBTNClick);
            if (ClearButton)
                ClearButton.onClick.AddListener(_On_ClearMessageBTNClick);
        }

        void Start()
        {

        }

        void Update()
        {

        }
    }
}