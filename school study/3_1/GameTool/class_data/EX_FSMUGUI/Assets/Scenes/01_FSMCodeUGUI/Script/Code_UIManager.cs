using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Du3Project
{
    

    public class Code_UIManager : MonoBehaviour
	{
        StateMachine m_UIGameMode = new StateMachine();

        void Awake()
        {
            // gameobject active가 안되어있으면 find가 안됨
            //UI_WorldMap testmap = GameObject.FindObjectOfType<UI_WorldMap>();
            //Test_Mono testmono = GameObject.FindObjectOfType<Test_Mono>();

            //IState worldmap = GameObject.FindObjectOfType<UI_WorldMap>() as IState;
            UI_WorldMap worldmap = GameObject.FindObjectOfType<UI_WorldMap>();
            worldmap.gameObject.SetActive(false);
            m_UIGameMode.Add("worldmap", worldmap);

            UI_Battle battle = GameObject.FindObjectOfType<UI_Battle>();
            battle.gameObject.SetActive(false);
            m_UIGameMode.Add("battle", battle);


            UI_Option option = GameObject.FindObjectOfType<UI_Option>();
            option.gameObject.SetActive(false);
            m_UIGameMode.Add("option", option);


            UI_FieldMap fieldmap = GameObject.FindObjectOfType<UI_FieldMap>();
            fieldmap.gameObject.SetActive(false);
            m_UIGameMode.Add("fieldmap", fieldmap);


            SetChangeState();
            //m_UIGameMode.Change(m_CurrentStateType.ToString(), null);


        }


        public E_UIState m_CurrentStateType = E_UIState.worldmap;
        public void SetChangeState()
        {
            m_UIGameMode.Change(m_CurrentStateType.ToString(), null);
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
            m_UIGameMode.UpdateState(Time.time);

        }
	}
}