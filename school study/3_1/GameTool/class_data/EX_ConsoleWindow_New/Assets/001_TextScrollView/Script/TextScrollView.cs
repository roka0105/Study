using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Text.RegularExpressions;
using UnityEngine.UI;
using UnityEngine.EventSystems;

namespace Du3Project
{
    public class TextScrollView : MonoBehaviour
    {
        const string LINE_SPLIT_RE = @"\r\n|\n\r|\n|\r";
        public Text TextLabel = null;
        public ScrollRect ScrollTextView = null;
        public InputField InputFieldText = null;

        public bool ISUpdate = false;
        public float VerticalNormalizedPosition = 0f;

        public string m_MsgText = "";


        private void Awake()
        {
            m_MsgText = TextLabel.text;

        }

        void Start()
        {

        }

        void Update()
        {
            if (ISUpdate)
            {
                Canvas.ForceUpdateCanvases();
                ScrollTextView.verticalNormalizedPosition = VerticalNormalizedPosition;
                ISUpdate = false;
            }

        }


        void SetSelectInputField()
        {
            if (false)
            {
                // 기본 inputfield 선택 방법
                InputFieldText.Select();
                InputFieldText.ActivateInputField();
            }
            else
            {
                // 모든것 선택하는 방식
                EventSystem.current.SetSelectedGameObject(InputFieldText.gameObject, null);
                InputFieldText.OnPointerClick(new PointerEventData(EventSystem.current));
            }
        }

        // InputField > OnEndEdit 에 등록 하도록 하기
        public void _On_InputFieldEnterKey(string p_str)
        {
            // _On_InputFieldEnterKey 2번 호출됨
            // Input.GetKey(KeyCode.Return); 2번다 사실이됨
            // Input.GetKeyDown( KeyCode.Return); 이용해서 한번만 호출 되도록 하기
            bool isflag = Input.GetKey(KeyCode.Return);
            bool isflag2 = Input.GetKeyDown(KeyCode.Return);

            Debug.LogFormat("InputField : {0}, {1}, {2}", p_str, isflag, isflag2);

            if (InputFieldText.text.Length > 0
                && (Input.GetKeyDown(KeyCode.KeypadEnter) || Input.GetKeyDown(KeyCode.Return))
                )
            {
                m_MsgText += "\n" + Regex.Replace(InputFieldText.text, LINE_SPLIT_RE, "");
                InputFieldText.text = "";
                UpdateLabel();

                SetSelectInputField();
            }
        }

        public void _On_InputClickEvent()
        {
            string tempstr = "\n" + InputFieldText.text;
            if (tempstr != "")
            {
                m_MsgText += tempstr;
            }

            InputFieldText.text = "";

            UpdateLabel();
        }

        void UpdateLabel()
        {
            Canvas.ForceUpdateCanvases();
            TextLabel.text = m_MsgText;
            ISUpdate = true;
            Canvas.ForceUpdateCanvases();

        }
    }
}