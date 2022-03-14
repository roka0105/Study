using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Du3Project
{
	public class UI_InGameManager : SceneStageBase<E_UIState>
	{
        // 싱글톤 만들기
        #region 싱글톤


        protected static UI_InGameManager instance;

        /**
          Returns the instance of this singleton.
       */
        public static UI_InGameManager GetI
        {
            get
            {
                if (instance == null)
                {
                    instance = GameObject.FindObjectOfType<UI_InGameManager>();

                    if (instance == null)
                    {
                        Debug.LogError("-- An instance of : " + typeof(UI_InGameManager) +
                                       "\n is needed in the scene, but there is none.");
                    }
                }

                return instance;
            }
        }


        #endregion





        public UI_BattleForm UIBattleForm = null;
        public UI_WorldMapForm UIWorldMapForm = null;
        public UI_OptionForm UIOptionForm = null;
        protected UI_FieldMapForm UIFieldMapForm = null;


        //protected List<ElementBaseForm<E_UIState>> m_AllUIFormList = new List<ElementBaseForm<E_UIState>>();
        protected override ElementBaseForm<E_UIState> GetScenePanelCom(E_UIState p_scenetype)
        {
            m_CurrentSceneStagePanel = null;
            m_CurrentSceneStagePanel = m_AllUIFormList[(int)p_scenetype];

            if (m_CurrentSceneStagePanel == null)
            {
                Debug.LogErrorFormat("Null PanelCom : {0}", p_scenetype);
            }
            else
            {

            }

            return m_CurrentSceneStagePanel;
        }

        void InitSettingUIForm()
        {
            // 직접 찾기도 하는 방법이 있음
            UIFieldMapForm = GetComponentInChildren<UI_FieldMapForm>();

            for (int i=0; i<(int)E_UIState.Max; ++i)
            {
                m_AllUIFormList.Add(null);
            }

            m_AllUIFormList[(int)E_UIState.battle] = UIBattleForm;
            m_AllUIFormList[(int)E_UIState.worldmap] = UIWorldMapForm;
            m_AllUIFormList[(int)E_UIState.option] = UIOptionForm;
            m_AllUIFormList[(int)E_UIState.fieldmap] = UIFieldMapForm;

            for (int i=0; i< (int)E_UIState.Max; ++i)
            {
                m_AllUIFormList[i].gameObject.SetActive(false);
            }

        }
        void Awake()
        {
            InitSettingUIForm();
        }

        void Start()
		{
            SetChangeScene(CurrentSceneStateType, true);

		}

	}
}