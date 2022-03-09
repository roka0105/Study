using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace FSMGUI_NUM1
{
    public class UI_Manager : MonoBehaviour
    {
        StateMachine m_UIGameMode = new StateMachine();
        public E_UIState CurrentState = E_UIState.world;
        void Awake()
        {
            UI_Battle Battle = GameObject.FindObjectOfType<UI_Battle>();
            Battle.gameObject.SetActive(false);
            m_UIGameMode.Add("battle", Battle);
            UI_Field Field = GameObject.FindObjectOfType<UI_Field>();
            Field.gameObject.SetActive(false);
            m_UIGameMode.Add("field", Field);
            UI_Option Option = GameObject.FindObjectOfType<UI_Option>();
            Option.gameObject.SetActive(false);
            m_UIGameMode.Add("option", Option);
            UI_World World = GameObject.FindObjectOfType<UI_World>();
            World.gameObject.SetActive(false);
            m_UIGameMode.Add("world", World);

            SetChangeState();
        }

        public void SetChangeState()
        {
            Debug.Log(CurrentState.ToString());
            m_UIGameMode.Change(CurrentState.ToString(), null);
        }
        [ContextMenu("[상태변환]")]
        void Editor_ChangeState()
        {
            SetChangeState();
        }
        void Start()
        {

        }

        void Update()
        {
            if(m_UIGameMode.StateNullCheck)
            m_UIGameMode.UpdateState(Time.time);
        }
    }
}
