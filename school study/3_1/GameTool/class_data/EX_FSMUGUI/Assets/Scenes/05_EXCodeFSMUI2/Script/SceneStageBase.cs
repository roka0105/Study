using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace Du3Project
{

    public abstract class SceneStageBase<TEnum>
    : MonoBehaviour
    {
        // 현재 씬파일 로드할수 있도록 하기 위한것임
        // 하단에서 꼭만들어야지됨
        protected abstract ElementBaseForm<TEnum> GetScenePanelCom(TEnum p_scenetype);

            // 참고용 소스
        //protected override ElementBaseForm<E_UIState> GetScenePanelCom(E_UIState p_scenetype)
        //{
        //    m_CurrentSceneStagePanel = null;
        //    m_CurrentSceneStagePanel = m_AllUIFormList[(int)p_scenetype];

        //    if (m_CurrentSceneStagePanel == null)
        //    {
        //        Debug.LogErrorFormat("Null PanelCom : {0}", p_scenetype);
        //    }
        //    else
        //    {

        //    }

        //    return m_CurrentSceneStagePanel;
        //}


        protected List<ElementBaseForm<TEnum>> m_AllUIFormList = new List<ElementBaseForm<TEnum>>();

        protected ElementBaseForm<TEnum> m_CurrentSceneStagePanel = null;
        [SerializeField]
        protected TEnum m_CurrentSceneStateType;
        public TEnum CurrentSceneStateType
        {
            get { return m_CurrentSceneStateType; }
        }


        public virtual void SetChangeScene(TEnum p_scene, bool p_direct = false)
        {
            if (m_CurrentSceneStateType.GetHashCode() == p_scene.GetHashCode()
                && !p_direct)
            {
                return;
            }


            StopSceneChangeCoroutine();
            ChangeDirectPanelScene(p_scene);
        }


        Coroutine m_SceneChangeCoroutine = null;
        protected virtual void StopSceneChangeCoroutine()
        {
            if (m_SceneChangeCoroutine != null)
            {
                StopCoroutine(m_SceneChangeCoroutine);
                m_SceneChangeCoroutine = null;
            }

        }

        protected virtual void ChangeDirectPanelScene(TEnum p_scene)
        {
            if (m_CurrentSceneStagePanel != null)
            {
                m_CurrentSceneStagePanel.LeaveForm();
            }

            ChangePanelScene(p_scene);
        }



        protected virtual void ChangePanelScene(TEnum p_scene)
        {
            TEnum temppage = m_CurrentSceneStateType;


            m_CurrentSceneStateType = p_scene;
            m_CurrentSceneStagePanel = null;


            m_CurrentSceneStagePanel = GetScenePanelCom(p_scene);


            if (m_CurrentSceneStagePanel)
            {
                m_CurrentSceneStagePanel.EnterForm();
            }
            else
            {
                Debug.LogError(" Over Page : " + p_scene);
            }
        }






        void Start()
        {

        }




    }


}