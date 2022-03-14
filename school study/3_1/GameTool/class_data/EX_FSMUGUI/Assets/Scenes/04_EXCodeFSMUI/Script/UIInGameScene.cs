using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Du3Project
{
	public class UIInGameScene : SceneStageBase<E_UIState>
	{
        public UIBattle UIBattleCom = null;
        public UIFieldMap UIFieldMapCom = null;
        public UIOption UIOptionCom = null;
        public UIWorldMap UIWorldCom = null;


        ElementBaseForm<E_UIState>[] m_PanelUIComArr = new ElementBaseForm<E_UIState>[(int)E_UIState.Max];
        public ElementBaseForm<E_UIState>[] PanelUIComArr
        {
            get { return m_PanelUIComArr; }
        }

        protected override ElementBaseForm<E_UIState> GetScenePanelCom(E_UIState p_scenetype)
        {
            m_CurrentSceneStagePanel = null;
            m_CurrentSceneStagePanel = m_PanelUIComArr[(int)p_scenetype];

            // 추가적인 작업위한 부분용
            if (m_CurrentSceneStagePanel == null)
            {
                Debug.LogErrorFormat("Null PanelCom : {0}", p_scenetype);
            }
            else
            {

            }

            return m_CurrentSceneStagePanel;
        }

        bool m_ISInit = false;
        public void InitSettingSceneCom()
        {
            if (m_ISInit)
            {
                return;
            }

            // 초기화용
            m_PanelUIComArr[(int)E_UIState.worldmap] = UIWorldCom;
            m_PanelUIComArr[(int)E_UIState.fieldmap] = UIFieldMapCom;
            m_PanelUIComArr[(int)E_UIState.option] = UIOptionCom;
            m_PanelUIComArr[(int)E_UIState.battle] = UIBattleCom;


            AllHidePanel();

            m_ISInit = true;


            // 강제 적용
            SetChangeScene(E_UIState.worldmap, true);
        }

        public void AllHidePanel()
        {
            int count = m_PanelUIComArr.Length;
            for (int i = 0; i < count; ++i)
            {
                if (m_PanelUIComArr[i] != null)
                    m_PanelUIComArr[i].gameObject.SetActive(false);
            }

        }

        void Awake()
        {
            InitSettingSceneCom();

        }
        void Start()
		{
			
		}



        [ContextMenu("[강제 수정하기]")]
        void Editor_ChangeSceneType()
        {
            SetChangeScene(m_CurrentSceneStateType, true);
        }


        // 현재로써는 구현하기위해서는 많은 부분을 바꿔야지됨
        //[ContextMenu("[자동판넬 연결]")]
        //void Editor_AutoPanelSetting()
        //{

        //}


		//void Update()
		//{
			
		//}
	}
}