using System;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using System.Text.RegularExpressions;
using UnityEngine;

namespace Du3Project
{
    public class Console_RuntimeObj : BaseConsoleWindow
    {
        public HintWindow LinkHintWindow = null;
        public HintWindow LinkChildHintWindow = null;


        [Space(20f)]
        [Header("[확인용]")]
        [SerializeField]
        protected List<string> m_InGameObjectList = new List<string>();

        [SerializeField]
        protected List<string> foundGameObjectNames = new List<string>();
        [SerializeField]
        protected List<string> foundComponentNames = new List<string>();
        [SerializeField]
        private List<string> foundMethodNames = new List<string>();
        [SerializeField]
        private List<string> foundFieldNames = new List<string>();
        [SerializeField]
        private List<string> foundPropertyNames = new List<string>();

        protected float m_DelayFrequencySec = 20f;


        private string m_PrevInputField = "";
        private bool showingAutocomplete = false;

        IEnumerator FindAllGameObjects()
        {
            while (Application.isPlaying)
            {
                FindAllGameList();
                yield return new WaitForSeconds(m_DelayFrequencySec);
            }

            yield return null;
        }

        public void FindAllGameList()
        {
            m_InGameObjectList.Clear();

            int layerindex = LayerMask.NameToLayer("UI");

            GameObject[] gameobjectarr = GameObject.FindObjectsOfType(typeof(GameObject)) as GameObject[];
            foreach (GameObject go in gameobjectarr)
            {
                if (go.layer != layerindex)
                    m_InGameObjectList.Add(go.name);
            }
        }

        public void _On_ChangeInputFieldText(string p_str)
        {
            // 이곳에서 호출이 되면 문자열 파싱쪽에서 이상하게 디버깅 정보를 알수가 없었음
            // 2017의 버그인지 파악해야지됨 현재 사용에는 지장이 없어서 inputfield의 이벤트를 이용해서 처리하고 있음
            UpdateRuntimeConsole();

        }

        /// <summary>
        /// /cmd 라는 입력필드를 받게되면 
        /// </summary>
        protected void UpdateRuntimeConsole()
        {
            foundGameObjectNames.Clear();
            foundComponentNames.Clear();
            foundMethodNames.Clear();
            foundFieldNames.Clear();
            foundPropertyNames.Clear();
            showingAutocomplete = false;

            string workWithText = inputField.text;
            m_PrevInputField = workWithText;

            //Debug.LogFormat("Text : {0}", workWithText);
            bool printResult = false;
            if (workWithText.ToLower().StartsWith("/cmd "))
            {
                workWithText = workWithText.Substring(5, workWithText.Length - 5);
                printResult = true;
            }


            if (printResult)
            {
                string[] splitSpaces = workWithText.Split(' ');
                if (splitSpaces.Length > 0)
                {
                    string[] splitDots = workWithText.Split('.');

                    GameObject go = null;
                    if (splitDots.Length == 1)
                    {
                        // 같은 이름 오브젝트 스트링 찾기
                        foundGameObjectNames.AddRange(m_InGameObjectList.FindAll(
                            delegate (string goName)
                            {
                                return goName.ToLower().StartsWith(splitDots[0].ToLower());
                            }
                            ));

                    }

                    // 오브젝트의 컴포넌트들 찾기및 등록 하기
                    if (splitDots.Length == 2)
                    {
                        go = GameObject.Find(splitDots[0]);
                        if (go != null)
                        {
                            Component[] goComps = go.GetComponents<Component>();
                            for (int cI = 0; cI < goComps.Length; cI++)
                            {
                                string cTypeName = goComps[cI].GetType().ToString();
                                string[] splitCTypeName = cTypeName.Split('.');
                                cTypeName = splitCTypeName[splitCTypeName.Length - 1];
                                if (cTypeName.ToLower().StartsWith(splitDots[1].ToLower()))
                                {
                                    foundComponentNames.Add(cTypeName);
                                }
                            }
                        }

                    }

                    // 해당 컴포넌트의 함수, 필드, 프로퍼티 찾기
                    if (splitDots.Length == 3)
                    {
                        go = GameObject.Find(splitDots[0]);

                        if (go != null)
                        {
                            Component component = go.GetComponent(splitDots[1]) as Component;
                            if (component != null)
                            {
                                MethodInfo[] mInfos = (component.GetType()).GetMethods();
                                foreach (MethodInfo mInfo in mInfos)
                                {
                                    if (mInfo.Name.ToLower().StartsWith(splitDots[2].ToLower()))
                                    {
                                        if (!foundMethodNames.Contains(mInfo.Name))
                                            foundMethodNames.Add(mInfo.Name);
                                    }
                                }
                                FieldInfo[] fInfos = (component.GetType()).GetFields();
                                foreach (FieldInfo fInfo in fInfos)
                                {
                                    if (fInfo.Name.ToLower().StartsWith(splitDots[2].ToLower()))
                                    {
                                        if (!foundFieldNames.Contains(fInfo.Name))
                                            foundFieldNames.Add(fInfo.Name);
                                    }
                                }
                                PropertyInfo[] pInfos = (component.GetType()).GetProperties();
                                foreach (PropertyInfo pInfo in pInfos)
                                {
                                    if (pInfo.Name.ToLower().StartsWith(splitDots[2].ToLower()))
                                    {
                                        if (!foundPropertyNames.Contains(pInfo.Name))
                                            foundPropertyNames.Add(pInfo.Name);
                                    }
                                }
                            }
                        }
                    }
                }

            }

            // . 의 갯수에 따라서 함수들이 지정이됨
            if (foundGameObjectNames.Count > 0 || foundComponentNames.Count > 0 || foundMethodNames.Count > 0)
            {
                showingAutocomplete = true;
                ShowHintWindow();
            }
            else
            {
                HideWindow();
            }

        }





        List<string> m_TempParamParsingList = new List<string>();
        List<string> m_TempParamList = new List<string>();
        // InputField 엔터로 입력받은값용
        protected override void CallInputFieldEndConsoleAddLabel(string p_str)
        {
            base.CallInputFieldEndConsoleAddLabel(p_str);

            // 문자열 파싱해서 명령어 실행하도록 하기

            bool printResult = false;
            string workWithText = p_str.Trim();

            if (workWithText.ToLower().StartsWith("/cmd "))
            {
                workWithText = workWithText.Substring(5, workWithText.Length - 5);
                printResult = true;
            }


            if (printResult)
            {
                string[] splitSpaces = workWithText.Split(' ');
                if (splitSpaces.Length > 0)
                {
                    string[] splitDots = workWithText.Split('.');

                    GameObject go = null;
                    Component component = null;

                    //MethodInfo methodInfo = null;
                    //FieldInfo fieldInfo = null;
                    //PropertyInfo propertyInfo = null;

                    if (splitDots.Length >= 1)
                    {
                        go = GameObject.Find(splitDots[0]);
                    }

                    // 오브젝트의 컴포넌트들 찾기및 등록 하기
                    if (go && splitDots.Length >= 2)
                    {
                        component = go.GetComponent(splitDots[1]) as Component;
                    }

                    // 해당 컴포넌트의 함수, 필드, 프로퍼티 찾기
                    if (component && splitDots.Length == 3)
                    {
                        var result = Regex.Split(splitDots[2], " ");

                        m_TempParamParsingList.Clear();
                        m_TempParamParsingList.AddRange(result);

                        m_TempParamList.Clear();
                        m_TempParamList.AddRange(result);
                        m_TempParamList.RemoveAt(0);

                        RunTypeCall(go, component, m_TempParamParsingList[0], m_TempParamList.ToArray());
                        //RunMethod(go, component, m_TempParamParsingList[0], m_TempParamList.ToArray(), false);
                    }
                }

            }
        }

        void RunTypeCall(GameObject gameObject
            , Component component
            , string callname
            , string[] arguments)
        {
            bool ismatching = false;
            if (component != null)
            {
                MethodInfo[] mInfos = (component.GetType()).GetMethods();
                foreach (MethodInfo mInfo in mInfos)
                {
                    if (mInfo.Name.StartsWith(callname))
                    {
                        RunMethod(gameObject, component, callname, arguments, false);
                        ismatching = true;
                        break;
                    }
                }
                if (!ismatching)
                {
                    FieldInfo[] fInfos = (component.GetType()).GetFields();
                    foreach (FieldInfo fInfo in fInfos)
                    {
                        if (fInfo.Name.StartsWith(callname))
                        {
                            if (arguments.Length == 0)
                            {
                                GetField(gameObject, component, callname);
                            }
                            else
                            {
                                // 필드 방식은 무조건 하나의 변수만 적용하기때문
                                SetField(gameObject, component, callname, arguments[0]);
                            }

                            ismatching = true;
                            break;
                        }
                    }
                }

                if (!ismatching)
                {
                    PropertyInfo[] pInfos = (component.GetType()).GetProperties();
                    foreach (PropertyInfo pInfo in pInfos)
                    {
                        if (pInfo.Name.StartsWith(callname))
                        {
                            if (arguments.Length == 0)
                            {
                                GetProperty(gameObject, component, callname);
                            }
                            else
                            {
                                // 프로퍼티 값도 하나의 값만 적용되기때문임
                                SetProperty(gameObject, component, callname, arguments[0]);
                            }

                            ismatching = true;
                            break;
                        }
                    }
                }

            }

        }


        // 매소드 강제 호출 방법
        void RunMethod(GameObject gameObject
            , Component component
            , string methodName
            , string[] arguments
            , bool printResult)
        {
            Type componentType = component.GetType();
            MethodInfo[] mInfos = componentType.GetMethods();

            object[] pMatchedArguments = new object[0];

            int mostMatchingParams = -1;
            int bestMatchNumParams = 0;
            int bestMatch = -1;
            bool willReturn = false;
            object result = null;
            for (int i = 0; i < mInfos.Length; i++)
            {
                MethodInfo mInfo = mInfos[i];

                int numMatchingParams = 0;

                if (mInfo != null)
                {
                    if (mInfo.Name != methodName)
                    {
                        continue;
                    }

                    bool paramSuccess = true;


                    // 매소드에서 필요한 파라메타들을 타입별로 알아내어 arguments 값을 파싱하도록처리
                    // Find arg types
                    ParameterInfo[] pInfo = mInfo.GetParameters();

                    object[] pArguments = new object[pInfo.Length];

                    for (int pI = 0; pI < pInfo.Length; pI++)
                    {
                        if (arguments.Length > pI)
                        {
                            // int 형
                            if (pInfo[pI].ParameterType == typeof(int))
                            {
                                int intResult = 0;
                                paramSuccess = int.TryParse(arguments[pI], out intResult);
                                if (paramSuccess)
                                {
                                    pArguments[pI] = intResult;
                                    numMatchingParams++;
                                }
                            }
                            // float 형
                            else if (pInfo[pI].ParameterType == typeof(float))
                            {
                                float floatResult = 0;
                                paramSuccess = float.TryParse(arguments[pI], out floatResult);
                                if (paramSuccess)
                                {
                                    pArguments[pI] = floatResult;
                                    numMatchingParams++;
                                }
                            }
                            else if (pInfo[pI].ParameterType == typeof(double))
                            {
                                double doubleResult = 0;
                                paramSuccess = double.TryParse(arguments[pI], out doubleResult);
                                if (paramSuccess)
                                {
                                    pArguments[pI] = doubleResult;
                                    numMatchingParams++;
                                }
                            }
                            else if (pInfo[pI].ParameterType == typeof(bool))
                            {
                                if (arguments[pI].ToUpper() == "FALSE")
                                {
                                    pArguments[pI] = false;
                                    numMatchingParams++;
                                }
                                else if (arguments[pI].ToUpper() == "TRUE")
                                {
                                    pArguments[pI] = true;
                                    numMatchingParams++;
                                }
                                else
                                {
                                    int intResult = 0;
                                    paramSuccess = int.TryParse(arguments[pI], out intResult);
                                    if (paramSuccess)
                                    {
                                        pArguments[pI] = (intResult == 0) ? false : true;
                                        numMatchingParams++;
                                    }
                                }
                            }
                            else if (pInfo[pI].ParameterType == typeof(string))
                            {
                                pArguments[pI] = arguments[pI];
                                numMatchingParams++;
                            }
                            // Vector2 형
                            else if (pInfo[pI].ParameterType == typeof(Vector2))
                            {
                                string[] splitArg = arguments[pI].Split(',');
                                if (splitArg.Length == 2)
                                {
                                    float xResult = 0f;
                                    bool xSuccess = false;
                                    xSuccess = float.TryParse(splitArg[0], out xResult);

                                    float yResult = 0f;
                                    bool ySuccess = false;
                                    ySuccess = float.TryParse(splitArg[2], out yResult);

                                    if (xSuccess && ySuccess)
                                    {
                                        pArguments[pI] = new Vector2(xResult, yResult);
                                        numMatchingParams++;
                                    }
                                    else
                                    {
                                        paramSuccess = false;
                                    }
                                }
                            }
                            // Vector3 형
                            else if (pInfo[pI].ParameterType == typeof(Vector3))
                            {
                                string[] splitArg = arguments[pI].Split(',');
                                if (splitArg.Length == 3)
                                {
                                    float xResult = 0f;
                                    bool xSuccess = false;
                                    xSuccess = float.TryParse(splitArg[0], out xResult);

                                    float yResult = 0f;
                                    bool ySuccess = false;
                                    ySuccess = float.TryParse(splitArg[1], out yResult);

                                    float zResult = 0f;
                                    bool zSuccess = false;
                                    zSuccess = float.TryParse(splitArg[2], out zResult);

                                    if (xSuccess && ySuccess && zSuccess)
                                    {
                                        pArguments[pI] = new Vector3(xResult, yResult, zResult);
                                        numMatchingParams++;
                                    }
                                    else
                                    {
                                        paramSuccess = false;
                                    }
                                }
                            }
                            else if (pInfo[pI].ParameterType == typeof(Quaternion))
                            {
                                string[] splitArg = arguments[pI].Split(',');
                                if (splitArg.Length == 4)
                                {
                                    float xResult = 0f;
                                    bool xSuccess = false;
                                    xSuccess = float.TryParse(splitArg[0], out xResult);

                                    float yResult = 0f;
                                    bool ySuccess = false;
                                    ySuccess = float.TryParse(splitArg[1], out yResult);

                                    float zResult = 0f;
                                    bool zSuccess = false;
                                    zSuccess = float.TryParse(splitArg[2], out zResult);

                                    float wResult = 0f;
                                    bool wSuccess = false;
                                    wSuccess = float.TryParse(splitArg[2], out wResult);

                                    if (xSuccess && ySuccess && zSuccess && wSuccess)
                                    {
                                        pArguments[pI] = new Quaternion(xResult, yResult, zResult, wResult);
                                        numMatchingParams++;
                                    }
                                    else
                                    {
                                        paramSuccess = false;
                                    }
                                }
                            }
                            else if (pInfo[pI].ParameterType == typeof(Color))
                            {
                                string[] splitArg = arguments[pI].Split(',');
                                if (splitArg.Length == 4)
                                {
                                    float xResult = 0f;
                                    bool xSuccess = false;
                                    xSuccess = float.TryParse(splitArg[0], out xResult);

                                    float yResult = 0f;
                                    bool ySuccess = false;
                                    ySuccess = float.TryParse(splitArg[1], out yResult);

                                    float zResult = 0f;
                                    bool zSuccess = false;
                                    zSuccess = float.TryParse(splitArg[2], out zResult);

                                    float wResult = 0f;
                                    bool wSuccess = false;
                                    wSuccess = float.TryParse(splitArg[2], out wResult);

                                    if (xSuccess && ySuccess && zSuccess && wSuccess)
                                    {
                                        pArguments[pI] = new Color(xResult, yResult, zResult, wResult);
                                        numMatchingParams++;
                                    }
                                    else
                                    {
                                        paramSuccess = false;
                                    }
                                }
                            }
                        }
                        else
                        {
                            // 구조체 형태등은 파싱할수 있는 방법을 추가적으로 만들어 두어야지됨
                            // 클래스 형태를 받아낼수 없음
                            if (pInfo[pI].DefaultValue != null)
                            {
                                paramSuccess = false;
                            }
                            else
                            {
                                pArguments[pI] = pInfo[pI].DefaultValue;
                            }
                        }

                        if (!paramSuccess) break;
                    }

                    // 반환 타입이 무엇이냐를 알아내어 반환타입값을 출력할수 있도록 적용
                    if (numMatchingParams > mostMatchingParams || ((numMatchingParams == mostMatchingParams) && (bestMatchNumParams > pInfo.Length)))
                    {
                        bestMatchNumParams = pInfo.Length;
                        mostMatchingParams = numMatchingParams;
                        bestMatch = i;
                        pMatchedArguments = pArguments;
                        if (mInfo.ReturnType != typeof(void))
                        {
                            willReturn = true;
                        }
                        else
                        {
                            willReturn = false;
                        }
                    }
                }
            }

            if (bestMatch >= 0)
            {
                if (willReturn)
                {
                    result = mInfos[bestMatch].Invoke(component, pMatchedArguments);
                    AppendText("결과 : " + result.ToString());
                    //if (printResult || true)
                    //{
                    //}
                }
                else
                {
                    mInfos[bestMatch].Invoke(component, pMatchedArguments);
                }
            }
        }






        void GetField(GameObject gameObject, Component component, string fieldName)
        {
            Type componentType = component.GetType();
            FieldInfo fInfo = componentType.GetField(fieldName);

            if (fInfo != null)
            {
                // GetValue 이용해 필드값 알아내기
                AppendText(fInfo.GetValue(component).ToString());
            }
        }

        void SetField(GameObject gameObject, Component component, string fieldName, string argument)
        {
            Type componentType = component.GetType();
            FieldInfo fInfo = componentType.GetField(fieldName);

            if (fInfo != null)
            {
                bool paramSuccess = true;
                object pArgument = null;

                paramSuccess = ParseStringToObject(fInfo.FieldType, argument, ref pArgument);

                if (paramSuccess)
                {
                    // SetValue 이용해 값 지정하기
                    fInfo.SetValue(component, pArgument);
                }
                else
                {
                    AppendText("Field 에 세팅할 자료형이 틀립니다. 파싱이안되었습니다. : " + fInfo.FieldType.ToString());
                }
            }
        }

        void GetProperty(GameObject gameObject, Component component, string propertyName)
        {
            Type componentType = component.GetType();
            PropertyInfo pInfo = componentType.GetProperty(propertyName);

            if (pInfo != null)
            {
                AppendText(pInfo.GetValue(component, null).ToString());
            }
        }

        void SetProperty(GameObject gameObject, Component component, string propertyName, string argument)
        {
            Type componentType = component.GetType();
            PropertyInfo pInfo = componentType.GetProperty(propertyName);

            if (pInfo != null)
            {
                bool paramSuccess = true;
                object pArgument = null;

                paramSuccess = ParseStringToObject(pInfo.PropertyType, argument, ref pArgument);

                if (paramSuccess)
                {
                    pInfo.SetValue(component, pArgument, null);
                }
                else
                {
                    AppendText("프로퍼티에 적용할 값이 틀린자료형입니다. 파싱이안되었습니다. : " + pInfo.PropertyType.ToString());
                }
            }
        }

        bool ParseStringToObject(Type objectType, string argument, ref object returnObject)
        {
            bool paramSuccess = false;

            if (objectType == typeof(int))
            {
                int intResult = 0;
                paramSuccess = int.TryParse(argument, out intResult);
                if (paramSuccess)
                {
                    returnObject = intResult;
                    return true;
                }
                return false;
            }
            else if (objectType == typeof(float))
            {
                float floatResult = 0;
                paramSuccess = float.TryParse(argument, out floatResult);
                if (paramSuccess)
                {
                    returnObject = floatResult;
                    return true;
                }
                return false;
            }
            else if (objectType == typeof(double))
            {
                double doubleResult = 0;
                paramSuccess = double.TryParse(argument, out doubleResult);
                if (paramSuccess)
                {
                    returnObject = doubleResult;
                    return true;
                }
                return false;
            }
            else if (objectType == typeof(bool))
            {
                if (argument.ToUpper() == "FALSE")
                {
                    returnObject = false;
                    return true;
                }
                else if (argument.ToUpper() == "TRUE")
                {
                    returnObject = true;
                    return true;
                }
                else
                {
                    int intResult = 0;
                    paramSuccess = int.TryParse(argument, out intResult);
                    if (paramSuccess)
                    {
                        returnObject = (intResult == 0) ? false : true;
                        return true;
                    }
                    return false;
                }
            }
            else if (objectType == typeof(string))
            {
                returnObject = argument;
                return true;
            }
            else if (objectType == typeof(Vector2))
            {
                string[] splitArg = argument.Split(',');
                if (splitArg.Length == 2)
                {
                    float xResult = 0f;
                    bool xSuccess = false;
                    xSuccess = float.TryParse(splitArg[0], out xResult);

                    float yResult = 0f;
                    bool ySuccess = false;
                    ySuccess = float.TryParse(splitArg[2], out yResult);

                    if (xSuccess && ySuccess)
                    {
                        returnObject = new Vector2(xResult, yResult);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            else if (objectType == typeof(Vector3))
            {
                string[] splitArg = argument.Split(',');
                if (splitArg.Length == 3)
                {
                    float xResult = 0f;
                    bool xSuccess = false;
                    xSuccess = float.TryParse(splitArg[0], out xResult);

                    float yResult = 0f;
                    bool ySuccess = false;
                    ySuccess = float.TryParse(splitArg[1], out yResult);

                    float zResult = 0f;
                    bool zSuccess = false;
                    zSuccess = float.TryParse(splitArg[2], out zResult);

                    if (xSuccess && ySuccess && zSuccess)
                    {
                        returnObject = new Vector3(xResult, yResult, zResult);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            else if (objectType == typeof(Quaternion))
            {
                string[] splitArg = argument.Split(',');
                if (splitArg.Length == 4)
                {
                    float xResult = 0f;
                    bool xSuccess = false;
                    xSuccess = float.TryParse(splitArg[0], out xResult);

                    float yResult = 0f;
                    bool ySuccess = false;
                    ySuccess = float.TryParse(splitArg[1], out yResult);

                    float zResult = 0f;
                    bool zSuccess = false;
                    zSuccess = float.TryParse(splitArg[2], out zResult);

                    float wResult = 0f;
                    bool wSuccess = false;
                    wSuccess = float.TryParse(splitArg[3], out wResult);

                    if (xSuccess && ySuccess && zSuccess && wSuccess)
                    {
                        returnObject = new Quaternion(xResult, yResult, zResult, wResult);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            else if (objectType == typeof(Rect))
            {
                string[] splitArg = argument.Split(',');
                if (splitArg.Length == 4)
                {
                    float xResult = 0f;
                    bool xSuccess = false;
                    xSuccess = float.TryParse(splitArg[0], out xResult);

                    float yResult = 0f;
                    bool ySuccess = false;
                    ySuccess = float.TryParse(splitArg[1], out yResult);

                    float wResult = 0f;
                    bool wSuccess = false;
                    wSuccess = float.TryParse(splitArg[2], out wResult);

                    float hResult = 0f;
                    bool hSuccess = false;
                    hSuccess = float.TryParse(splitArg[3], out hResult);

                    if (xSuccess && ySuccess && wSuccess && hSuccess)
                    {
                        returnObject = new Rect(xResult, yResult, wResult, hResult);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            else if (objectType == typeof(Color))
            {
                string[] splitArg = argument.Split(',');
                if (splitArg.Length == 4)
                {
                    float xResult = 0f;
                    bool xSuccess = false;
                    xSuccess = float.TryParse(splitArg[0], out xResult);

                    float yResult = 0f;
                    bool ySuccess = false;
                    ySuccess = float.TryParse(splitArg[1], out yResult);

                    float zResult = 0f;
                    bool zSuccess = false;
                    zSuccess = float.TryParse(splitArg[2], out zResult);

                    float wResult = 0f;
                    bool wSuccess = false;
                    wSuccess = float.TryParse(splitArg[2], out wResult);

                    if (xSuccess && ySuccess && zSuccess && wSuccess)
                    {
                        returnObject = new Color(xResult, yResult, zResult, wResult);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }

            return false;
        }


        void HideWindow()
        {
            LinkHintWindow.gameObject.SetActive(false);
            LinkChildHintWindow.gameObject.SetActive(false);
        }

        void SetHintWindowCallFN(HintWindow p_window, string p_callstr, bool p_islast)
        {
            HideWindow();
            SetLastDotInputFieldSetting(p_callstr, p_islast);
        }


        void SetLastDotInputFieldSetting(string p_callstr, bool p_islast)
        {
            string callstr = inputField.text;
            string cmdstr = "";
            //inputField.selectionFocusPosition

            int dotindex = callstr.LastIndexOf('.');
            if (dotindex <= -1)
            {
                // 기본 명령어만 있음 스페이스로 적용된것 지우도록 하기
                int cmdindex = callstr.LastIndexOf(' ');
                cmdstr = callstr.Substring(0, cmdindex);
                cmdstr = string.Format("{0} {1}", cmdstr, p_callstr);
            }
            else
            {
                cmdstr = callstr.Substring(0, dotindex);
                cmdstr = string.Format("{0}.{1}", cmdstr, p_callstr);
            }

            if (!p_islast)
                cmdstr += ".";
            else
                cmdstr += " ";

            inputField.text = cmdstr;
            SetFocusInputField();

            // InputField 범위 방식
            //inputField.selectionFocusPosition = inputField.text.Length; // 시작위치
            //inputField.selectionAnchorPosition = inputField.text.Length; // 끝나는 위치
            inputField.caretPosition = inputField.text.Length;
            UpdateRuntimeConsole();
        }

        List<string> m_LinkNames = new List<string>();
        void ShowHintWindow()
        {
            bool hasMethodNames = foundMethodNames.Count > 0;
            bool hasFieldNames = foundFieldNames.Count > 0;
            bool hasPropertyNames = foundPropertyNames.Count > 0;

            HideWindow();
            if (hasMethodNames || hasFieldNames || hasPropertyNames)
            {
                LinkHintWindow.gameObject.SetActive(true);

                if (hasMethodNames)
                {
                    LinkHintWindow.DrawItems("Methods", foundMethodNames, true);
                }

                if (hasFieldNames || hasPropertyNames)
                {
                    LinkChildHintWindow.gameObject.SetActive(true);

                    m_LinkNames.Clear();
                    m_LinkNames.AddRange(foundFieldNames);
                    m_LinkNames.AddRange(foundPropertyNames);

                    LinkChildHintWindow.DrawItems("Fields & Properties", m_LinkNames, true);
                }

            }
            else if (foundComponentNames.Count > 0)
            {
                LinkHintWindow.gameObject.SetActive(true);
                LinkHintWindow.DrawItems("Compoment", foundComponentNames, false);
            }
            else if (foundGameObjectNames.Count > 0)
            {
                LinkHintWindow.gameObject.SetActive(true);
                LinkHintWindow.DrawItems("GameObject", foundGameObjectNames, false);
            }
        }


        #region 테스트 소스

        [Header("[테스트]")]
        public string TestCmdStr = "/cmd Cube.Tra";
        public string TestCallStr = "Transform";

        [ContextMenu("[테스트 필드]")]
        void _Editor_InputFieldData()
        {
            inputField.text = TestCmdStr;
            SetLastDotInputFieldSetting(TestCallStr, false);
        }

        [SerializeField]
        protected string TestCallCmdStr = "/cmd Cube.Transform.set_position 1,2,3";
        //protected string TestCallCmdStr = "/cmd Cube.Transform.get_position 1,2,3"; 
        // vector3 형일때는 1, 2, 3 형태는 파싱하도록 만들어야지됨 현재는 1,2,3 형태로 스페이스 값이 없도록 해야지됨 안그러면 파싱 정보가 짤리게됨
        // 파라메타 파싱은 기본적으로 ' ' 스페이스로 이루어짐
        // "/cmd Cube.Transform.set_position 1,2,3 abc 2, 3"; 형태이면 파라매터는 [ "1,2,3", "abc", "2,", "3" ] 형태의 값으로 된다

        [ContextMenu("[2.명령어 파싱 테스트]")]
        void _Eidtor_TestConsoleLabel()
        {
            CallInputFieldEndConsoleAddLabel(TestCallCmdStr);
        }

        #endregion


        void InitHintWindow()
        {
            LinkHintWindow.InitWindow(SetHintWindowCallFN);
            LinkChildHintWindow.InitWindow(SetHintWindowCallFN);

            HideWindow();
        }
        private void Awake()
        {
            inputField.onValueChanged.AddListener(_On_ChangeInputFieldText);
            InitHintWindow();
            InitSetting();
        }

        void Start()
        {
            StartCoroutine(FindAllGameObjects());
        }

        void Update()
        {
            UIUpdateConsole();

            // 디버깅시 이쪽이용해서 디버깅을 하면 문자열 파싱정보가잘보임
            //if(m_PrevInputField != inputField.text)
            //{
            //    UpdateRuntimeConsole();
            //}
        }


    }
}