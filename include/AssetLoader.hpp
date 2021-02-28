#pragma once
#include <vector>
#include "Types/Hat/Hat.hpp"
#include "Types/Material/Material.hpp"

namespace GorillaCosmetics
{
    class AssetLoader
    {
        public:
            static Material SelectedMaterial();
            static Material SelectedInfectedMaterial();
            static Hat SelectedHat();

            static void Load();
            static bool IsLoaded();
    
        private:
            static int SelectedHatFromConfig(std::string configString);
            static int SelectedMaterialFromConfig(std::string configString);

            static inline bool Loaded = false;
            static inline bool Loading = false;

            static const inline std::string FolderPath = "/sdcard/ModData/com.AnotherAxiom.GorillaTag/Mods/GorillaCosmetics/";
            static const inline std::string MirrorLocation = "Mirror/";
            static const inline std::string RackLocation = "Rack/";
            static const inline std::string MaterialsLocation = "Materials/";
            static const inline std::string HatsLocation = "Hats/";

            static inline int selectedMaterial = 0;
            static inline int selectedInfectedMaterial = 0; // TODO: better selection with UI
            static inline int selectedHat = 0;

            static inline std::vector<std::string> MaterialFiles = {};
            static inline std::vector<std::string> HatFiles = {};
            static inline std::vector<Material> GorillaMaterialObjects = {};
            static inline std::vector<Hat> GorillaHatObjects = {};

            template<class T>
            static void LoadItems(std::vector<std::string>& files, std::vector<T>& out)
            {
                for (auto f : files)
                {
                    out.push_back(T(f));
                }
                for (auto& o : out)
                {
                    o.Load();
                }
            }
    };
}