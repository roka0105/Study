using UnityEngine;
using System.Collections;

namespace Du3Project
{

    // public interface ElementBaseForm<TEnum> : MonoBehaviour
    // {
    // 
    //
    /// <summary>
    /// 폼들 만들때 씬들바뀔때 사용하기 위한 base용 함수
    /// TEnum 사용해서 해당씬에 대한 정보등을 얻을수 있도록 만들었음
    /// 
    /// 기본 사용법
    /// public class InGamePanelUI_Com : ElementBaseForm<E_InGameUI>
    /// {
    ///     public override E_InGameUI EnumType
    ///     {
    ///         get { return E_InGameUI.E_InGameUI; }
    ///     }
    /// }
    /// 
    /// </summary>
    /// <typeparam name="TEnum"></typeparam>
    abstract public class ElementBaseForm<TEnum> : MonoBehaviour// where TEnum : System.Enum
    {
        //protected TEnum m_EnumType;
        public abstract TEnum EnumType
        {
            get;// { return m_EnumType; }
        }


        public virtual void EnterForm()
        {
            Debug.LogFormat("Enter Form : {0}, {1}, {2}"
                , this.name
                , this.gameObject.activeSelf
                , this.gameObject.activeInHierarchy);


            this.gameObject.SetActive(true);
            //         if (!this.gameObject.activeSelf)
            //             this.gameObject.SetActive(true);
        }

        public virtual void LeaveForm()
        {
            this.gameObject.SetActive(false);
            //         if ( this.gameObject.activeSelf )
            //             this.gameObject.SetActive(false);
        }



        public virtual void ElementAnimationStartEvent()
        {

        }
        public virtual void ElementAnimationEndEvent()
        {

        }



        public float LeaveDelaySec = 0f;

    }


}