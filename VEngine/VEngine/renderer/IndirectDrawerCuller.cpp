#include "IndirectDrawerCuller.h"

using namespace vengine;

void IndirectDrawerCuller::add_entity(unsigned int meshId, unsigned int ssboId)
{
    unsigned int index = m_mesh_lod_to_dib_index.at(meshId);

    // Setting instance count
    unsigned int numInstances = 0;
    change_instance_count(numInstances, index, 1);

    // Setting gl_InstanceId
    unsigned int instanceId = numInstances - 1;
    m_ssboId_to_instanceId.insert(m_ssboId_to_instanceId.begin() + ssboId, instanceId);
    m_dib_instanceId_to_ssboId.at(index).emplace(instanceId, ssboId);
}

void IndirectDrawerCuller::init_ssbo(ShaderStorageManager* shaderStorageManager)
{
    auto dib = m_mutable_dib->get_persistent_map(m_num_objects - 1);
    m_dib_to_entity_data = &shaderStorageManager->make_mutable_shader_storage<unsigned int, GL_SHADER_STORAGE_BUFFER, 3>(
        "drawStuff", dib->prim_count + dib->base_instance, SpecialSyncType::TRIPPLE_BUFFER_SPECIAL_SYNC);
}
void IndirectDrawerCuller::set_ssbo(unsigned int meshId, unsigned int ssboId)
{
    const unsigned int dibIndex = m_mesh_lod_to_dib_index.at(meshId);
    const unsigned int index = m_mutable_dib->get_persistent_map(dibIndex)->base_instance + m_ssboId_to_instanceId.at(ssboId);
    m_dib_to_entity_data->set_initial_data(index, ssboId); // Special function
}

// Changing the draw indirect buffer
void IndirectDrawerCuller::change_instance_count(unsigned int dibIndex, int byHowMuch)
{
    unsigned int& numInstances = m_mutable_dib->get_persistent_map(dibIndex)->prim_count;
    numInstances += byHowMuch;

    // Setting gl_BaseInstance
    for (int i = dibIndex + 1; i < m_num_objects; i++)
    {
        m_mutable_dib->get_persistent_map(i)->base_instance += byHowMuch;
    }
}
void IndirectDrawerCuller::change_instance_count(unsigned int& _numInstances, unsigned int dibIndex, int byHowMuch)
{
    unsigned int& numInstances = m_mutable_dib->get_persistent_map(dibIndex)->prim_count;
    numInstances += byHowMuch;

    // Setting gl_BaseInstance
    for (int i = dibIndex + 1; i < m_num_objects; i++)
    {
        m_mutable_dib->get_persistent_map(i)->base_instance += byHowMuch;
    }

    _numInstances = numInstances;
}

// Culling

void IndirectDrawerCuller::uncull_entities_batched(const std::vector<unsigned int>& meshIds, const std::vector<unsigned int>& ssboIds)
{
    // const auto& dib = m_mutable_dib->get_persistent_map();

    std::map<unsigned int, std::vector<unsigned int>> ordered_ssboIds;
    for (int i = 0; i < m_num_objects; i++)
    {
        ordered_ssboIds.emplace(i, std::vector<unsigned int>(0));
    }

    for (int i = 0; i < meshIds.size(); i++)
    {
        auto& meshId = meshIds.at(i);
        auto& ssboId = ssboIds.at(i);

        const unsigned int index = m_mesh_lod_to_dib_index.at(meshId);
        const unsigned int numInstances = m_mutable_dib->get_persistent_map(index)->prim_count;

        unsigned int numEntitiesAdded = ordered_ssboIds.at(index).size();
        m_ssboId_to_instanceId.at(ssboId) = numInstances + numEntitiesAdded; // numInstnaces isnt an index but since we are adding to the
                                                                             // end, that implicit numInstancesIndex + 1 is useful

        // TEMP
        auto& dibToEntityId = m_dib_instanceId_to_ssboId.at(index);
        if (dibToEntityId.find(numInstances + numEntitiesAdded) == dibToEntityId.end())
        {
            dibToEntityId.emplace(numInstances + numEntitiesAdded, ssboId); // Haven't made my mind up on this yet
        }
        m_dib_instanceId_to_ssboId.at(index).at(numInstances + numEntitiesAdded) = ssboId;

        ordered_ssboIds.at(index).push_back(ssboId);
    }

    unsigned int       sum = 0;
    const auto         dib1 = m_mutable_dib->get_persistent_map(m_num_objects - 1);
    const unsigned int maxFeasibleValue = dib1->base_instance + dib1->prim_count;
    for (int i = 0; i < m_num_objects - 1; i++)
    {
        const auto         dib = m_mutable_dib->get_persistent_map(i);
        const unsigned int toCpyFrom = dib->prim_count + dib->base_instance;
        auto&              ordered_ssbos = ordered_ssboIds.at(i);

        if (ordered_ssbos.size() == 0)
            continue;

        m_dib_to_entity_data->insert_data_virtual_max(ordered_ssbos, toCpyFrom + sum, maxFeasibleValue + sum);

        sum += ordered_ssbos.size();
    }

    auto& ordered_ssbos = ordered_ssboIds.at(m_num_objects - 1);
    if (ordered_ssbos.size() != 0)
    {
        m_dib_to_entity_data->insert_data_no_shift(ordered_ssbos, maxFeasibleValue + sum);
    }

    for (int i = 0; i < m_num_objects; i++)
    {
        const unsigned int numInstancesAdded = ordered_ssboIds.at(i).size();
        change_instance_count(i, numInstancesAdded);
    }
}
void IndirectDrawerCuller::uncull_entities(unsigned int meshId, unsigned int ssboId)
{
    // const auto& dib = m_mutable_dib->get_persistent_map();

    const unsigned int index = m_mesh_lod_to_dib_index.at(meshId);
    const unsigned int gl_BaseInstance = m_mutable_dib->get_persistent_map(index)->base_instance;
    // As this will increase due to changeInstanceCount down below
    const unsigned int maxFeasibleValue = m_mutable_dib->get_persistent_map(m_num_objects - 1)->base_instance +
                                          m_mutable_dib->get_persistent_map(m_num_objects - 1)->prim_count + 1;

    // Setting instance count
    unsigned int numInstances = 0;
    change_instance_count(numInstances, index, 1);

    const unsigned int currentIndex = numInstances + gl_BaseInstance - 1;

    if (currentIndex < maxFeasibleValue - 1)
    {

        m_dib_to_entity_data->shift_data_virtual_max(currentIndex, currentIndex + 1,
                                                     maxFeasibleValue); // As numInstances and so on Increase
    }

    *m_dib_to_entity_data->get_updatebuf_persistent_map(currentIndex) = ssboId;
    m_ssboId_to_instanceId.at(ssboId) = numInstances - 1;
    m_dib_instanceId_to_ssboId.at(index).at(numInstances - 1) = ssboId;
}

void IndirectDrawerCuller::cull_entities_batched(const std::vector<unsigned int>& meshIds, const std::vector<unsigned int>& ssboIds)
{
    // const auto&        dib = m_mutable_dib->get_persistent_map();
    const unsigned int maxFeasibleValue = m_mutable_dib->get_persistent_map(m_num_objects - 1)->base_instance +
                                          m_mutable_dib->get_persistent_map(m_num_objects - 1)->prim_count;
    unsigned int currentIndex = 0;
    unsigned int endIndex = 0;

    std::vector<unsigned int> numEntitiesRemovedPerDib;
    std::vector<unsigned int> baseInstancePerDib, numInstancesPerDib;
    for (int i = 0; i < m_num_objects; i++)
    { // This is actually faster
        const auto dib = m_mutable_dib->get_persistent_map(i);
        numInstancesPerDib.push_back(dib->prim_count);
        baseInstancePerDib.push_back(dib->base_instance);
    }
    numEntitiesRemovedPerDib.resize(m_num_objects);
    for (int i = 0; i < meshIds.size(); i++)
    {
        auto& meshId = meshIds.at(i);
        auto& ssboId = ssboIds.at(i);

        const unsigned int index = m_mesh_lod_to_dib_index.at(meshId);
        const unsigned int numInstances = numInstancesPerDib.at(index); // Slow maybe?
        const unsigned int gl_BaseInstance = baseInstancePerDib.at(index);
        const unsigned int gl_InstanceID = m_ssboId_to_instanceId.at(ssboId);
        unsigned int&      numEntitiesRemoved = numEntitiesRemovedPerDib.at(index);

        currentIndex = gl_BaseInstance + gl_InstanceID;
        // Problem for numInstances = 0;
        endIndex = gl_BaseInstance + numInstances - 1 - numEntitiesRemoved; //-1 initially to get the correct index, we decrease each time


        // entityIdToInstanceId.at(entityId) = 2 * maxIndex;
        if (currentIndex < endIndex)
        {

            unsigned int end = *m_dib_to_entity_data->get_updatebuf_persistent_map(endIndex);

            *m_dib_to_entity_data->get_updatebuf_persistent_map(endIndex) = ssboId;
            *m_dib_to_entity_data->get_updatebuf_persistent_map(currentIndex) = end;

            unsigned int& endSsboId = m_dib_instanceId_to_ssboId.at(index).at(numInstances - 1 - numEntitiesRemoved);
            m_ssboId_to_instanceId.at(endSsboId) = gl_InstanceID;
            m_dib_instanceId_to_ssboId.at(index).at(gl_InstanceID) = endSsboId;
        }

        numEntitiesRemoved++;
    }

    unsigned int sum = 0;
    for (int i = 0; i < m_num_objects - 1; i++)
    {
        const auto  dib = m_mutable_dib->get_persistent_map(i);
        const auto& numEntitiesRemoved = numEntitiesRemovedPerDib.at(i);
        const auto  toCpyFrom = dib->prim_count + dib->base_instance;

        m_dib_to_entity_data->shift_data_virtual_max(toCpyFrom - sum, toCpyFrom - sum - numEntitiesRemoved, maxFeasibleValue);

        sum += numEntitiesRemoved;
    }

    // Here we actually change instance count;
    for (int i = 0; i < m_num_objects; i++)
    {
        const int numInstancesRemoved = numEntitiesRemovedPerDib.at(i);
        change_instance_count(i, -numInstancesRemoved);
    }
}
void IndirectDrawerCuller::cull_entities(unsigned int meshId, unsigned int ssboId)
{
    // const auto&        dib = m_mutable_dib->get_persistent_map();
    const auto         dib1 = m_mutable_dib->get_persistent_map(m_num_objects - 1);
    const unsigned int maxFeasibleValue = dib1->base_instance + dib1->prim_count;
    const unsigned int index = m_mesh_lod_to_dib_index.at(meshId);
    const unsigned int gl_BaseInstance = m_mutable_dib->get_persistent_map(index)->base_instance;
    const unsigned int gl_InstanceID = m_ssboId_to_instanceId.at(ssboId);
    unsigned int       currentIndex = gl_BaseInstance + gl_InstanceID;

    // Setting instance count
    unsigned int numInstances = 0;
    change_instance_count(numInstances, index, -1);
    unsigned int endIndex = gl_BaseInstance + numInstances; // numInstances shrunk by 1 but index should be -1

    if (currentIndex < endIndex)
    {

        // Switching on persistent map
        unsigned int end = *m_dib_to_entity_data->get_updatebuf_persistent_map(endIndex);

        *m_dib_to_entity_data->get_updatebuf_persistent_map(endIndex) = ssboId;
        *m_dib_to_entity_data->get_updatebuf_persistent_map(currentIndex) = end;

        // Switching on our data
        unsigned int& endSsboId = m_dib_instanceId_to_ssboId.at(index).at(numInstances);
        m_ssboId_to_instanceId.at(endSsboId) = gl_InstanceID;
        m_dib_instanceId_to_ssboId.at(index).at(gl_InstanceID) = endSsboId;

        currentIndex = endIndex;
    }

    if (currentIndex < maxFeasibleValue - 1)
    {
        m_dib_to_entity_data->shift_data_virtual_max(currentIndex + 1, currentIndex, maxFeasibleValue);
    }
}
