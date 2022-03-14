using System.Collections;
using System.Collections.Generic;
using UnityEngine;


// 참고용 자료
// // https://docs.unity3d.com/kr/current/Manual/StateMachineBehaviours.html
namespace Du3Project
{
    [RequireComponent(typeof(Animator))]
	public class UIFSMAnimatorControl : MonoBehaviour
	{
        public Animator LinkAnimator = null;
        [SerializeField]
        private E_UIState m_CurrentUIState = E_UIState.Max;


        public UIFSMElement UIWorldFSMElement = null;
        public UIFSMElement UIBattleFSMElement = null;
        public UIFSMElement UIOptionFSMElement = null;
        public UIFSMElement UIFieldMapFSMElement = null;


        private UIFSMElement[] m_UIFSMElementComArray = new UIFSMElement[(int)E_UIState.Max];

        public void ChangeUGUI(E_UIState p_uistate)
        {
            if(m_CurrentUIState == p_uistate)
            {
                return;
            }

            m_CurrentUIState = p_uistate;
            LinkAnimator.SetTrigger(m_CurrentUIState.ToString());

        }

        void Awake()
        {
            if(LinkAnimator == null)
            {
                LinkAnimator = GetComponent<Animator>();
            }

            FSMANI_Worldmap worldmap = LinkAnimator.GetBehaviour<FSMANI_Worldmap>();
            worldmap.UILinkGameObject = UIWorldFSMElement;
            UIWorldFSMElement.gameObject.SetActive(false);

            FSMANI_Battle battle = LinkAnimator.GetBehaviour<FSMANI_Battle>();
            battle.UILinkGameObject = UIBattleFSMElement;
            UIBattleFSMElement.gameObject.SetActive(false);

            FSMANI_Option option = LinkAnimator.GetBehaviour<FSMANI_Option>();
            option.UILinkGameObject = UIOptionFSMElement;
            UIOptionFSMElement.gameObject.SetActive(false);

            FSMANI_FieldMap fieldmap = LinkAnimator.GetBehaviour<FSMANI_FieldMap>();
            fieldmap.UILinkGameObject = UIFieldMapFSMElement;
            UIFieldMapFSMElement.gameObject.SetActive(false);


            // 자동 완성 형태로바꿀수 있음
            // 아이디어는 FSMANI_Option 에서 옵션값을 받아올수 있음 그것으로 각가의 엘리멘트를 적용할수 있음


        }


        [ContextMenu("[스테이터스바꾸기]")]
        void Editor_SetChangeType()
        {
            //ChangeUGUI(m_CurrentUIState);
            LinkAnimator.SetTrigger(m_CurrentUIState.ToString());
        }

        void Start()
		{
			
		}

		//void Update()
		//{
			
		//}
	}
}