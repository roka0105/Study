using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;

namespace Du3Project
{
    [RequireComponent(typeof(ScrollRect))]
    public class HintWindow : MonoBehaviour
    {
        public Text HeadLabel = null;
        public Toggle ToggleItem = null;
        protected RectTransform m_ContentTransform = null;
        protected bool m_ISLastLabel = false;

        UnityAction<HintWindow, string, bool> m_CallbackFN = null;
        public void InitWindow(UnityAction<HintWindow, string, bool> p_callfn)
        {
            m_CallbackFN = p_callfn;
        }

        public void DrawItems(string p_headlabel, List<string> p_itemnamelist, bool p_islast)
        {
            this.gameObject.SetActive(true);
            m_ISLastLabel = p_islast;

            HeadLabel.text = p_headlabel;

            ClearAllDrawItems();
            int count = p_itemnamelist.Count;

            for (int i = 0; i < count; ++i)
            {
                CreateDrawItem(p_itemnamelist[i]);
            }

        }

        protected void ClearAllDrawItems()
        {
            int count = m_ContentTransform.childCount;
            GameObject destroyobj = null;
            for (int i = 0; i < count; ++i)
            {
                destroyobj = m_ContentTransform.GetChild(i).gameObject;
                if (destroyobj != ToggleItem.gameObject)
                    GameObject.Destroy(destroyobj);
            }
        }
        protected void CreateDrawItem(string p_label)
        {
            Toggle copyobj = GameObject.Instantiate<Toggle>(ToggleItem, m_ContentTransform);
            copyobj.gameObject.SetActive(true);
            Text label = copyobj.GetComponentInChildren<Text>();
            label.text = p_label;

            copyobj.onValueChanged.AddListener(
                delegate
                {
                    CallClickItem(copyobj);
                });

        }

        // 누가누른지에 대한 내용이 없음
        protected void CallClickItem(Toggle p_clickitem)
        {
            Text label = p_clickitem.GetComponentInChildren<Text>();
            Debug.LogFormat("ClickToggle : {0}", label.text);

            if (m_CallbackFN != null)
            {
                m_CallbackFN(this, label.text, m_ISLastLabel);
            }

            // 클릭했기때문에 닫도록 하기
        }

        private void Awake()
        {
            ToggleItem.gameObject.SetActive(false);
            m_ContentTransform = GetComponent<ScrollRect>().content;
        }
        void Start()
        {

        }

        //void Update()
        //{

        //}
    }
}