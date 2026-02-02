
//! BNM::IL2CPP::Il2CppObject if class inherits System.Object or nothing
//! BNM::UnityEngine::Object if class inherits UnityEngine.ScriptableObject
//! BNM::UnityEngine::MonoBehaviour if class inherits UnityEngine.MonoBehaviour

/*
// BNM::Method<ret type>
// BNM::Field<type>
        BNM::Class Display_c = BNM::Class("UnityEngine", "Display");
        BNM::Method<int> Display::getWidth = Display_c.GetMethod("get_systemWidth", 0);
*/

// Offsets, Fields, Classes, Properties...
namespace ofst {
    inline bool inited = false;
    
    void Init();
}
