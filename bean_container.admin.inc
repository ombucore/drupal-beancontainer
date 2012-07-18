<?php

/**
 * @file
 * Admin callbacks for bean container.
 */

/**
 * Manage container children form.
 */
function bean_container_manage_children($form, &$form_state, $bean_container) {
  drupal_set_title('Container children');

  if (!isset($form['#bean_container'])) {
    $form['#bean_container'] = $bean_container;
  }

  if (!empty($bean_container->children)) {
    $weight = 0;
    $form['children'] = array(
      '#tree' => TRUE,
    );
    foreach ($bean_container->children as $child) {
      $child_bean = bean_load($child);
      if (!$child_bean) {
        continue;
      }
      $form['children'][$child]['#bean'] = $child_bean;
      $form['children'][$child]['#weight'] = $weight;
      $form['children'][$child]['weight'] = array(
        '#type' => 'textfield',
        '#title' => t('Weight for @title', array('@title' => $child_bean->title)),
        '#title_display' => 'invisible',
        '#size' => 4,
        '#default_value' => $weight++,
        '#attributes' => array('class' => array('bean-container-weight')),
      );
    }
    $form['children']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );
  }

  return $form;
}

/**
 * Submit handler for manage children form.
 */
function bean_container_manage_children_submit($form, &$form_state) {
  $children = array();
  uasort($form_state['values']['children'], 'drupal_sort_weight');
  foreach ($form_state['values']['children'] as $bid => $values) {
    if (is_int($bid)) {
      $children[] = $bid;
    }
  }
  $form['#bean_container']->data['children'] = $children;
  $form['#bean_container']->save();

  drupal_set_message(t('Container children weights updated.'));
}

/**
 * Form for adding a block to a container bean.
 */
function bean_container_add_block($form, &$form_state, $bean_container) {
  // During initial form build, add the container entity to the form state for
  // useduring form building and processing. During a rebuild, use what is in
  // the form state.
  if (!isset($form_state['bean_container'])) {
    $form_state['bean_container'] = $bean_container;
  }
  else {
    $bean_container = $form_state['bean_container'];
  }

  if (empty($form_state['next_step'])) {
    $form['block_type'] = array(
      '#type' => 'select',
      '#title' => t('Block Type'),
      '#options' => array('' => 'Select One') + bean_containers_get_types(),
      '#description' => t("Choose the type of block you'd like to add"),
      '#required' => TRUE,
    );

    $form['next'] = array(
      '#type' => 'submit',
      '#value' => t('Next'),
      '#submit' => array('bean_container_add_form_next'),
    );
  }
  else {
    $bean = bean_create(array('type' => $form_state['type_values']['block_type']));
    $form_state['bean'] = $bean;
    module_load_include('inc', 'bean', 'includes/bean.pages');
    $form['bean_form'] = bean_form(array(), $form_state, $bean, $form_state['type_values']['block_type']);
    unset($form['bean_form']['actions']);

    // Hide the label.
    $form['bean_form']['label']['#access'] = FALSE;
    $form['bean_form']['label']['#default_value'] = 'ombucleanup-tempkey';

    $form['save'] = array(
      '#type' => 'submit',
      '#value' => t('Add'),
      '#validate' => array('bean_container_add_form_add_validate'),
      '#submit' => array('bean_container_add_form_add_submit'),
    );
  }

  return $form;
}

/**
 * Advance to the next step for the add a block form for bean containers.
 */
function bean_container_add_form_next($form, &$form_state) {
    $form_state['next_step'] = TRUE;
    $form_state['type_values'] = $form_state['values'];
    $form_state['rebuild'] = TRUE;
}

/**
 * Validate add a block form.
 */
function bean_container_add_form_add_validate($form, &$form_state) {
  if (isset($form_state['values']['bean'])) {
    $bean = $form_state['values']['bean'];

    $bean->label = $form_state['values']['label'];
    $bean->title = $form_state['values']['title'];

    if (isset($form_state['values']['view_mode'])) {
      $bean->view_mode = $form_state['values']['view_mode'];
    }

    field_attach_form_validate('bean', $bean, $form, $form_state);
    $form_state['values']['bean'] = $bean;

    $bean->validate($form_state['values'], $form_state);
  }
}

/**
 * Submit add a block form.
 */
function bean_container_add_form_add_submit($form, &$form_state) {
  module_load_include('inc', 'bean', 'includes/bean.pages');
  $bean = $form_state['bean'];
  entity_form_submit_build_entity('bean', $bean, $form, $form_state);

  // Transform temporary delta key into a readable key.
  if ($bean->label == 'ombucleanup-tempkey') {
    if ($bean->title) {
      $bean->label = $bean->title;
    }
    else {
      $bean->label = $bean->type;
    }
  }

  $max_length = 32;
  // Base it on the label and make sure it isn't too long for the database.
  $bean->delta = drupal_clean_css_identifier(strtolower($bean->label));
  $bean->delta = substr($bean->delta, 0, $max_length);

  // Check if delta is unique.
  if (bean_load_delta($bean->delta)) {
    $i = 0;
    $separator = '-';
    $original_delta = $bean->delta;
    do {
      $unique_suffix = $separator . $i++;
      $bean->delta = substr($original_delta, 0, $max_length - drupal_strlen($unique_suffix)) . $unique_suffix;
    } while (bean_load_delta($bean->delta));
  }

  foreach (module_implements('bean_submit') as $module) {
    $function = $module . '_bean_submit';
    $function($bean, $form, $form_state);
  }

  $bean->setValues($form_state['values']);
  field_attach_submit('bean', $bean, $form, $form_state);
  $bean->save();

  $watchdog_args = array('@type' => $bean->type, '%title' => $bean->label);
  $t_args = array('@type' => $bean->typeName(), '%title' => $bean->label);

  watchdog('bean', '@type: added %title.', $watchdog_args, WATCHDOG_NOTICE, $bean->viewURL());
  drupal_set_message(t('@type %title has been added to the container.', $t_args));

  // Assign new bean to bean container.
  $bean_container = $form_state['bean_container'];
  $bean_container->data['children'][] = $bean->bid;
  $bean_container->save();
}

/**
 * Theme container children as a reorderable list in the container bean edit
 * form.
 */
function theme_bean_container_manage_children($variables) {
  $form = $variables['form'];
  $output = '';

  if (isset($form['children'])) {
    $header = array(
      t('Title'),
      t('Type'),
      t('Weight'),
      array('data' => t('Operations'), 'colspan' => 2),
    );
    $rows = array();
    $destination = drupal_get_destination();
    foreach (element_children($form['children']) as $key) {
      if (isset($form['children'][$key]['#bean'])) {
        $bean = $form['children'][$key]['#bean'];
        $row = array();
        $row[] = check_plain($bean->title);
        $row[] = check_plain($bean->typeName());
        $row[] = drupal_render($form['children'][$key]['weight']);
        $row[] = l(t('edit'), $bean->editURL(), array('query' => $destination)) . ' ' . l(t('delete'), $bean->deleteURL(), array('query' => $destination));
        $rows[] = array('data' => $row, 'class' => array('draggable'));
      }
    }

    drupal_add_tabledrag('bean-container-children', 'order', 'sibling', 'bean-container-weight');

    $output = theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array('id' => 'bean-container-children'),
    ));
  }
  $output .= drupal_render_children($form);

  return $output;
}
