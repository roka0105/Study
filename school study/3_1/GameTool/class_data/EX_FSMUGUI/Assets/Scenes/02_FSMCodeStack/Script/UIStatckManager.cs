using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Du3Project
{
    // 기본적으로 완성된 소스가 아님 pop으로 해서 기존에 있던것들을 전부 사라지도록 해야지 되는데 현재 것은 그런 방식이 아님
	public class UIStatckManager : MonoBehaviour
	{
        [Header("[스테이트용]")]
        public UI_FieldMap FieldMapCom = null;
        public UI_WorldMap WorldCom = null;
        public UI_Option OptionCom = null;
        public UI_Battle BattleCom = null;



        public E_UIState CurrentUIState = E_UIState.worldmap;
        protected Code_StateStackMachine m_CodeStatckMachine = new Code_StateStackMachine();


        public void ChangeState(E_UIState p_change)
        {
            IState pushstate = m_CodeStatckMachine.Push(p_change.ToString());
            //(pushstate as MonoBehaviour).gameObject.SetActive(true);


        }

        void InitStateDatas()
        {
            m_CodeStatckMachine.Add(E_UIState.battle.ToString(), BattleCom);
            m_CodeStatckMachine.Add(E_UIState.worldmap.ToString(), WorldCom);
            m_CodeStatckMachine.Add(E_UIState.option.ToString(), OptionCom);
            m_CodeStatckMachine.Add(E_UIState.fieldmap.ToString(), FieldMapCom);

            BattleCom.gameObject.SetActive(false);
            WorldCom.gameObject.SetActive(false);
            OptionCom.gameObject.SetActive(false);
            FieldMapCom.gameObject.SetActive(false);
        }

        void Awake()
        {
            InitStateDatas();
            ChangeState(CurrentUIState);

        }

        void Start()
		{
			
		}

        void Update()
        {
            m_CodeStatckMachine.Update(Time.time);
            m_CodeStatckMachine.Render();
        }



        [ContextMenu("[UI추가]")]
        void Editor_AddChangeUI()
        {
            ChangeState(CurrentUIState);

        }


        [ContextMenu("[기존UI취소]")]
        void Editor_PopChangeUI()
        {
            IState popstate = m_CodeStatckMachine.Pop();
            (popstate as MonoBehaviour).gameObject.SetActive(false);

        }

    }
}