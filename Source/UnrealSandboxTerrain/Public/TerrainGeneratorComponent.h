#pragma once


// Copyright blackw 2015-2020

#pragma once

#include "EngineMinimal.h"
#include "VoxelData.h"
#include "VoxelIndex.h"
#include "SandboxTerrainCommon.h"
#include <unordered_map>
#include <mutex>
#include "TerrainGeneratorComponent.generated.h"


class TPerlinNoise;
class ASandboxTerrainController;
class TChunkHeightMapData;
struct TInstanceMeshArray;
struct FSandboxFoliage;
typedef TMap<int32, TInstanceMeshArray> TInstanceMeshTypeMap;


typedef struct TGenerateVdTempItm {

	int Idx = 0;

	TVoxelIndex ZoneIndex;
	TVoxelData* Vd;

	TChunkHeightMapData* ChunkData;

	// partial generation
	int GenerationLOD = 0;

	// partial generation v2
	bool bSlightGeneration = false;

} TGenerateVdTempItm;


typedef std::tuple<FVector, FVector, float, TMaterialId> ResultA;

/**
*
*/
UCLASS()
class UNREALSANDBOXTERRAIN_API UTerrainGeneratorComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

public:
		
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void FinishDestroy();

	ASandboxTerrainController* GetController() const;

	float PerlinNoise(const float X, const float Y, const float Z) const;

	float PerlinNoise(const FVector& Pos) const;

	float PerlinNoise(const FVector& Pos, const float PositionScale, const float ValueScale) const;

	void BatchGenerateVoxelTerrain(const TArray<TSpawnZoneParam>& GenerationList, TArray<TVoxelData*>& NewVdArray);

	virtual float GroundLevelFunction(const TVoxelIndex& Index, const FVector& V) const;

	virtual float DensityFunctionExt(float Density, const TVoxelIndex& ZoneIndex, const FVector& WorldPos, const FVector& LocalPos) const;

	int32 ZoneHash(const FVector& ZonePos);

	virtual void Clean();

	virtual void Clean(TVoxelIndex& Index);

	//========================================================================================
	// foliage etc.
	//========================================================================================

	virtual void GenerateInstanceObjects(const TVoxelIndex& Index, TVoxelData* Vd, TInstanceMeshTypeMap& ZoneInstanceMeshMap);

	virtual bool UseCustomFoliage(const TVoxelIndex& Index);

	virtual bool SpawnCustomFoliage(const TVoxelIndex& Index, const FVector& WorldPos, int32 FoliageTypeId, FSandboxFoliage FoliageType, FRandomStream& Rnd, FTransform& Transform);

	virtual FSandboxFoliage FoliageExt(const int32 FoliageTypeId, const FSandboxFoliage& FoliageType, const TVoxelIndex& ZoneIndex, const FVector& WorldPos);

	virtual bool OnCheckFoliageSpawn(const TVoxelIndex& ZoneIndex, const FVector& FoliagePos, FVector& Scale);

	virtual bool IsOverrideGroundLevel(const TVoxelIndex& Index);

	virtual float GeneratorGroundLevelFunc(const TVoxelIndex& Index, const FVector& Pos, float GroundLevel);

	virtual bool ForcePerformZone(const TVoxelIndex& ZoneIndex);

protected:

	TPerlinNoise* Pn;

	virtual void BatchGenerateComplexVd(TArray<TGenerateVdTempItm>& GenPass2List);

	virtual void OnBatchGenerationFinished();

private:

	TArray<FTerrainUndergroundLayer> UndergroundLayersTmp;

	std::mutex ZoneHeightMapMutex;

	std::unordered_map<TVoxelIndex, TChunkHeightMapData*> ChunkDataCollection;

	TChunkHeightMapData* GetChunkHeightMap(int X, int Y);

	virtual TVoxelDataFillState GenerateSimpleVd(const TVoxelIndex& ZoneIndex, TVoxelData* VoxelData, TChunkHeightMapData** ChunkDataPtr);

	bool IsZoneOverGroundLevel(TChunkHeightMapData* ChunkHeightMapData, const FVector& ZoneOrigin) const;

	bool IsZoneOnGroundLevel(TChunkHeightMapData* ChunkHeightMapData, const FVector& ZoneOrigin) const;

	float ClcDensityByGroundLevel(const FVector& V, const float GroundLevel) const;

	void GenerateZoneVolume(const TVoxelIndex& ZoneIndex, TVoxelData* VoxelData, const TChunkHeightMapData* ChunkHeightMapData, const int LOD = 0) const;

	FORCEINLINE TMaterialId MaterialFuncion(const FVector& LocalPos, const FVector& WorldPos, float GroundLevel) const;

	const FTerrainUndergroundLayer* GetMaterialLayer(float Z, float RealGroundLevel) const;

	int GetMaterialLayersCount(TChunkHeightMapData* ChunkHeightMapData, const FVector& ZoneOrigin, TArray<FTerrainUndergroundLayer>* LayerList) const;

	void GenerateNewFoliage(const TVoxelIndex& Index, TInstanceMeshTypeMap& ZoneInstanceMeshMap);

	void SpawnFoliage(int32 FoliageTypeId, FSandboxFoliage& FoliageType, const FVector& Origin, FRandomStream& rnd, const TVoxelIndex& Index, TInstanceMeshTypeMap& ZoneInstanceMeshMap);

	void GenerateNewFoliageCustom(const TVoxelIndex& Index, TVoxelData* Vd, TInstanceMeshTypeMap& ZoneInstanceMeshMap);

	//====

	ResultA A(const TVoxelIndex& Index, TVoxelData* VoxelData, const TChunkHeightMapData* ChunkData) const;

};